#ifndef ARDRONE_PROXY_SERIAL_H
#define ARDRONE_PROXY_SERIAL_H

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <vector>

namespace arproxy {
  class Serial {
    public:
      Serial(void);
      ~Serial(void);
      
      void process(void);
      
      bool receive(std::vector<char>& packet, size_t& bytes);
      bool send(const std::vector<char>& packet, size_t bytes);
      
      void send_handler(const boost::system::error_code& error, size_t bytes);
      void receive_handler(const boost::system::error_code& error, size_t bytes);
      
      std::vector<char> send_buffer, receive_buffer;
      
    private:
      bool sending, receiving;
      boost::asio::io_service io;
      boost::asio::serial_port* port;
      size_t received_bytes;
      
      //boost::asio::streambuf receive_buffer;      
  };
}

#endif
