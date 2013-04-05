#include <iostream>
#include <termios.h>
#include <boost/asio/serial_port.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <signal.h>
#include <sstream>
#include "serial.h"
#include "network.h"
#include "protocol.h"
using namespace std;

bool stop = false;
void interrupt(int s) {
  stop = true;
}

int main(void)
{
  cout << "Starting ardrone-proxy" << endl;
  signal(SIGINT, interrupt);

  arproxy::Serial serial;
  arproxy::Network network;
  
  size_t buffer_size = std::max(serial.receive_buffer.size(), network.receive_buffer.size());
  vector<char> recv_buffer(buffer_size), send_buffer(buffer_size);
    
  arproxy::ProtocolHandler protocol_handler;
  while(!stop) {
    serial.process();
    network.process();
  
    size_t recv_bytes, send_bytes;
    if (serial.receive(recv_buffer, recv_bytes)) {
      protocol_handler.serial2network(recv_buffer, recv_bytes, send_buffer, send_bytes);
      if (send_bytes != 0) {
        cout << "asking network to send packet" << endl;
        if (!network.send(send_buffer, send_bytes)) // TODO: may discard!
          cerr << "outbound network message of " << send_bytes << " dropped" << endl;
      }
    }
    
    /*if (network.receive(recv_buffer, recv_bytes)) {
      protocol_handler.network2serial(recv_buffer, recv_bytes, send_buffer, send_bytes);
      if (!serial.send(send_buffer, send_bytes))
        cerr << "outbound serial message of " << send_bytes << " dropped" << endl;
    }*/
    
    usleep(1000); // TODO: tune
  }
  
  return 0;
}
