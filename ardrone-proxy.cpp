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

void daemonize(void);

int main(int argc, char** argv)
{
  bool do_daemonize = true;
  if (argc > 1) {
    if (std::string(argv[1]) == "-F") do_daemonize = false;
    else { cerr << "ardrone-proxy [-F]" << endl << "  Use -F to avoid sending process to background" << endl; return 1; }
  }
    
  cout << "Starting ardrone-proxy" << endl;
  signal(SIGINT, interrupt);
  signal(SIGTERM, interrupt);
  
  if (do_daemonize) {
    cout << "sending process to background" << endl;
    daemonize();
  }

  arproxy::Serial serial;
  arproxy::Network network;
  
  size_t buffer_size = std::max(serial.receive_buffer.size(), network.receive_buffer.size());
  vector<char> recv_buffer(buffer_size), send_buffer(buffer_size);
  
  cout << "Server started" << endl;
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
  
  cout << "Server ended" << endl;  
  return 0;
}

void daemonize(void)
{
  /* Fork off the parent process */  
  pid_t pid = fork();  
  if (pid < 0) exit(EXIT_FAILURE); /* fork failed */
  if (pid > 0) exit(EXIT_SUCCESS); /*Killing the Parent Process*/  
  
  /* At this point we are executing as the child process */  

  /* Create a new SID for the child process */  
  pid_t sid = setsid();  
  if (sid < 0) exit(EXIT_FAILURE);  

  /* Change the current working directory. */  
  if ((chdir("/")) < 0) exit(EXIT_FAILURE);  

  int fd = open("/dev/null",O_RDWR, 0);
  if (fd != -1) {  
    dup2(fd, STDIN_FILENO);  
    dup2(fd, STDOUT_FILENO);  
    dup2(fd, STDERR_FILENO);  
    if (fd > 2) close(fd);  
  }
}
