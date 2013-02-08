#ifndef ARDRONE_PROXY_SERIAL_H
#define ARDRONE_PROXY_SERIAL_H

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

namespace arproxy {
  class Serial {
    public:
      Serial(void);
      ~Serial(void);
      
      void process(void);
      
      bool receive(std::string& msg);
      void send(const std::vector<char>& data, size_t bytes);
      
      void send_handler(const boost::system::error_code& error, size_t bytes);
      void receive_handler(const boost::system::error_code& error, size_t bytes);
      
    public:
      std::string received_msg;
      
    private:
      bool sending, receiving;
      boost::asio::io_service io;
      boost::asio::serial_port* port;
      std::vector<char> send_buffer;
      
      boost::asio::streambuf receive_buffer;      
  };
}

#endif
