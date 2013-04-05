#ifndef ARDRONE_PROXY_NETWORK_H
#define ARDRONE_PROXY_NETWORK_H

#include <boost/asio.hpp>
#include <vector>

namespace arproxy {
  class Network {
    public:
      Network(void);
      ~Network(void);
      
      void process(void);
      bool send(const std::vector<char>& packet, size_t bytes);
      bool receive(std::vector<char>& packet, size_t& bytes);
      
      void send_handler(const boost::system::error_code& error, size_t bytes);
      void receive_handler(const boost::system::error_code& error, size_t bytes);
      
      std::vector<char> receive_buffer, send_buffer;
      
    private:      
      bool sending, receiving;
      boost::asio::ip::udp::socket* at_socket, *nav_socket;
      boost::asio::ip::udp::endpoint at_receiver_endpoint, nav_receiver_endpoint;
      boost::asio::io_service io_service;
      
      
      size_t bytes_received;
  };
}

#endif
