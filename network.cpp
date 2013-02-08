#include <boost/bind.hpp>
#include "network.h"
using boost::asio::ip::udp;
using boost::asio::ip::address;

arproxy::Network::Network(void) : at_socket(NULL), nav_socket(NULL), receive_buffer(2048) {
  address drone_ip = address::from_string("192.168.1.1");
  at_receiver_endpoint = udp::endpoint(drone_ip, 5556);
  at_socket = new udp::socket(io_service);
  at_socket->open(udp::v4());
  at_socket->bind(udp::endpoint(drone_ip, 15556));
  
  nav_receiver_endpoint = udp::endpoint(drone_ip, 5554);
  nav_socket = new udp::socket(io_service);
  nav_socket->open(udp::v4());
  nav_socket->bind(udp::endpoint(drone_ip, 15554));
  
  sending = false;
  
  //async_read_(*at_socket, receive_buffer, '\n', boost::bind(&Serial::receive_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void arproxy::Network::process(void) {
  io_service.poll();
  if (io_service.stopped()) io_service.reset();
}

void arproxy::Network::send(const std::string& msg) {
  if (sending) return; // discard
  sending = true;
  send_buffer = msg;
  at_socket->async_send_to(boost::asio::buffer(send_buffer.c_str(), send_buffer.size()), at_receiver_endpoint, boost::bind(&Network::send_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
  
  // TODO: if (msg == "AT*NAV") mandar algo a nav_socket
}

/*void arproxy::Network::receive(void) {
  
}*/

void arproxy::Network::send_handler(const boost::system::error_code& error, size_t bytes) {
  sending = false;
}


/*void arproxy::Network::receive_handler(const boost::system::error_code& error, size_t bytes) {
  
}*/

arproxy::Network::~Network(void) {
  delete at_socket;
  delete nav_socket;
}
