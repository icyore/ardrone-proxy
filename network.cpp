#include <boost/bind.hpp>
#include "network.h"
using boost::asio::ip::udp;
using boost::asio::ip::address;
using namespace std;

#define ARDRONE_MAX_MESSAGE_SIZE 2048
#define ARDRONE_AT_PORT 5556
#define ARDRONE_NAV_PORT 5554

arproxy::Network::Network(void) : at_socket(NULL), nav_socket(NULL) {
  address drone_ip = address::from_string("192.168.1.1");
  at_receiver_endpoint = udp::endpoint(drone_ip, ARDRONE_AT_PORT);
  at_socket = new udp::socket(io_service);
  at_socket->open(udp::v4());
  at_socket->bind(udp::endpoint(drone_ip, ARDRONE_AT_PORT + 10000));
  
  nav_receiver_endpoint = udp::endpoint(drone_ip, ARDRONE_NAV_PORT);
  nav_socket = new udp::socket(io_service);
  nav_socket->open(udp::v4());
  nav_socket->bind(udp::endpoint(drone_ip, ARDRONE_NAV_PORT + 10000));
  
  bytes_received = 0;
  receiving = true;
  receive_buffer.resize(ARDRONE_MAX_MESSAGE_SIZE);
  nav_socket->async_receive(boost::asio::buffer(receive_buffer, ARDRONE_MAX_MESSAGE_SIZE),
    boost::bind(&Network::receive_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
  
  send_buffer.resize(ARDRONE_MAX_MESSAGE_SIZE);
  sending = false; // the followings sends are blocking
  int32_t one = 1;
  nav_socket->send_to(boost::asio::buffer((char*)&one, sizeof(int32_t)), nav_receiver_endpoint);
}

void arproxy::Network::process(void) {
  io_service.poll();
  if (io_service.stopped()) io_service.reset();
}

bool arproxy::Network::send(const std::vector<char>& packet, size_t bytes) {
  if (sending) return false; // discard
  std::copy(packet.begin(), packet.begin() + bytes, send_buffer.begin());
  //cout << "msg: " << std::string(&send_buffer[0], bytes) << endl;
  sending = true;
  at_socket->async_send_to(boost::asio::buffer(send_buffer, bytes), at_receiver_endpoint, boost::bind(&Network::send_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
  
  // TODO: if (msg == "AT*NAV") mandar algo a nav_socket
  return true;
}

bool arproxy::Network::receive(std::vector<char>& packet, size_t& bytes) {
  if (receiving || bytes_received == 0) return false;
  bytes = bytes_received;
  //cout << "copying " << bytes << " bytes to network buffer" << endl;
  std::copy(receive_buffer.begin(), receive_buffer.begin() + bytes, packet.begin());
  receiving = true;
  nav_socket->async_receive(boost::asio::buffer(receive_buffer, ARDRONE_MAX_MESSAGE_SIZE),
    boost::bind(&Network::receive_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
  return true;
}

void arproxy::Network::send_handler(const boost::system::error_code& error, size_t bytes) {
  /*if (!error) cout << "sent " << bytes << " bytes to nework" << endl;
  else cout << "error sending network data" << endl;*/
  sending = false;
}


void arproxy::Network::receive_handler(const boost::system::error_code& error, size_t bytes) {
  if (error || bytes == 0)
    bytes_received = 0;
  else {
    //cout << "received " << bytes << " from network" << endl;
    bytes_received = bytes;
  }

  receiving = false;  
}

arproxy::Network::~Network(void) {
  delete at_socket;
  delete nav_socket;
}
