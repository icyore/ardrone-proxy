#ifndef ARDRONE_PROXY_PROTOCOL_H
#define ARDRONE_PROXY_PROTOCOL_H

#include <vector>
#include <string>
#include "navdata.h"

namespace arproxy {
  class ProtocolHandler {
    public:
      ProtocolHandler(void);
      
      void serial2network(const std::vector<char>& serial_packet, size_t serial_bytes, std::vector<char>& network_packet, size_t& network_bytes);
      void network2serial(const std::vector<char>& network_packet, size_t network_bytes, std::vector<char>& serial_packet, size_t& serial_bytes);
      
      std::string prepare_network_msg(const std::string& msg, const std::string& parameters = std::string());
      uint32_t sequence_number, navdata_sequence_number;
      
    private:
      bool navdata_unpack(const navdata_t* navdata, navdata_demo_t* demo_data, navdata_cks_t* cks_data);
  };
}

#endif
