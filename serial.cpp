#include <boost/bind.hpp>
#include <iostream>
#include "mavlink/ardrone/mavlink.h"
#include "serial.h"
using namespace std;

arproxy::Serial::Serial(void) : port(NULL) {
  port = new boost::asio::serial_port(io, "/dev/ttyPA0");
  port->set_option(boost::asio::serial_port::baud_rate(115200));
  port->set_option(boost::asio::serial_port::character_size(8));
  port->set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::none));
  port->set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::none));
  port->set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));  
  
  receiving = true;
  sending = false;
  send_buffer.resize(MAVLINK_MAX_PACKET_LEN);
  receive_buffer.resize(MAVLINK_MAX_PACKET_LEN);
  
  // issue first receive
  received_bytes = 0;
  receiving = true;
  port->async_read_some(boost::asio::buffer(receive_buffer, MAVLINK_MAX_PACKET_LEN),
    boost::bind(&Serial::receive_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

arproxy::Serial::~Serial(void) {
  delete port;
}

void arproxy::Serial::process(void) {
  io.poll();
  if (io.stopped()) io.reset();
}

void arproxy::Serial::receive_handler(const boost::system::error_code& error, size_t bytes)
{
  cout << "received " << bytes << " bytes from serial" << endl;
  received_bytes = bytes;
  receiving = false;
}

bool arproxy::Serial::receive(std::vector<char>& packet, size_t& bytes) {
  if (receiving) return false;
  cout << "copying serial data to receive buffer" << endl;
  std::copy(receive_buffer.begin(), receive_buffer.begin() + received_bytes, packet.begin());
  bytes = received_bytes;
  receiving = true;
  port->async_read_some(boost::asio::buffer(receive_buffer, MAVLINK_MAX_PACKET_LEN),
    boost::bind(&Serial::receive_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
  return true;
}

bool arproxy::Serial::send(const vector<char>& packet, size_t bytes) {
  if (sending) { cout << "discarded" << endl; return false; } // discards data
  std::copy(packet.begin(), packet.begin() + bytes, send_buffer.begin());
  sending = true;
  boost::asio::async_write(*port, boost::asio::buffer(send_buffer, bytes), boost::bind(&Serial::send_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
  return true;
}



void arproxy::Serial::send_handler(const boost::system::error_code& error, size_t bytes) {
  cout << "sent " << bytes << " bytes" << endl;
  sending = false;
}

