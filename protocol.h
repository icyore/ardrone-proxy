#ifndef ARDRONE_PROXY_PROTOCOL_H
#define ARDRONE_PROXY_PROTOCOL_H

#include <vector>
#include <string>

namespace arproxy {
  class ProtocolHandler {
    public:
      ProtocolHandler(void);
      
      void serial2network(const std::vector<char>& serial_packet, size_t serial_bytes, std::vector<char>& network_packet, size_t& network_bytes);
      void network2serial(const std::vector<char>& network_packet, size_t network_bytes, std::vector<char>& serial_packet, size_t& serial_bytes);
      
      std::string prepare_network_msg(const std::string& msg, const std::string& parameters = std::string());
      int sequence_number;
  };
}

#endif
