#include <iostream>
#include <termios.h>
#include <boost/asio/serial_port.hpp>
#include <boost/asio.hpp>
#include <string>
#include <signal.h>
#include <sstream>
#include "serial.h"
#include "network.h"
using namespace std;

bool stop = false;
void interrupt(int s) {
  stop = true;
}

int main(void) {
  cout << "Starting ardrone-proxy" << endl;
  signal(SIGINT, interrupt);

  arproxy::Serial serial;
  arproxy::Network network;
  /*int seq_num = 1;
  ostringstream ostr;
  ostr << "AT*LED=" << seq_num << ",9,1065353216,5\r" << endl;
  string str(ostr.str());
  s->send_to(boost::asio::buffer(str.c_str(), str.size()), receiver_endpoint);*/
    
  while(!stop) {
    serial.process();
    network.process();
    
    string msg;
    if (serial.receive(msg)) {
      //cout << "received: " << msg.size() << " bytes" << endl;
      network.send(msg);
    }
    //else cout << "nothing received" << endl;
    
    /*ostringstream ostr;
    ostr << "AT*COMWDG=" << seq_num << "\r";
    //seq_num++;
    float f = 4;
    //ostr << "AT*LED=" << seq_num << ",1,1065353216,5\r" << endl;
    //3," << *(int32_t*)(&f) << ",1,100\r";
    
    cout << "sending" << endl;
    str = ostr.str();
    s->send_to(boost::asio::buffer(str.c_str(), str.size()), receiver_endpoint);
    cout << "sent" << endl;
    seq_num++;*/
    usleep(1000);
  }
  
  return 0;
}
