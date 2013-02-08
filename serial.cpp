#include <boost/bind.hpp>
#include <iostream>
#include "serial.h"
using namespace std;

arproxy::Serial::Serial(void) : port(NULL), receive_buffer(1024) {
  port = new boost::asio::serial_port(io, "/dev/ttyPA0");
  port->set_option(boost::asio::serial_port::baud_rate(115200));
  port->set_option(boost::asio::serial_port::character_size(8));
  port->set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::none));
  port->set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::none));
  port->set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));  
  
  receiving = true;
  sending = false;
  send_buffer.resize(2048);
  boost::asio::async_read_until(*port, receive_buffer, '\n', boost::bind(&Serial::receive_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

arproxy::Serial::~Serial(void) {
  delete port;
}

void arproxy::Serial::process(void) {
  io.poll();
  if (io.stopped()) io.reset();
}

void arproxy::Serial::send(const std::vector<char>& data, size_t bytes) {
  if (sending) { cout << "discarded" << endl; return; } // discards data
  
  sending = true;
  send_buffer = data;
  boost::asio::async_write(*port, boost::asio::buffer(send_buffer, bytes), boost::bind(&Serial::send_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

bool arproxy::Serial::receive(std::string& msg) {
  if (receiving) return false;
  msg = received_msg;  
  receiving = true;
  boost::asio::async_read_until(*port, receive_buffer, '\n', boost::bind(&Serial::receive_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
  return true;
}

void arproxy::Serial::receive_handler(const boost::system::error_code& error, size_t bytes) {
  cout << "[SERIAL] received msg of " << bytes << " bytes" << endl;
  std::istream is(&receive_buffer);
  std::getline(is, received_msg);
  receiving = false;
}

void arproxy::Serial::send_handler(const boost::system::error_code& error, size_t bytes) {
  sending = false;
}

