#ifndef ARDRONE_PROXY_NETWORK_H
#define ARDRONE_PROXY_NETWORK_H

#include <boost/asio.hpp>

namespace arproxy {
  class Network {
    public:
      Network(void);
      ~Network(void);
      
      void process(void);
      void send(const std::string& msg);
      
      void send_handler(const boost::system::error_code& error, size_t bytes);
      
    private:      
      bool sending;
      boost::asio::ip::udp::socket* at_socket, *nav_socket;
      boost::asio::ip::udp::endpoint at_receiver_endpoint, nav_receiver_endpoint;
      boost::asio::io_service io_service;
      
      std::string send_buffer;
      boost::asio::streambuf receive_buffer;            
  };
}

#endif
