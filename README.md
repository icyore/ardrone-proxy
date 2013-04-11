ARDrone Proxy
=============

This code enables the use of an on-board computer controlling the robot through the serial port of the ARDrone, instead of remotely (off-board) through wifi.

Dependencies
===========

This code requires the use of a gcc/g++ ARM cross-compiler. Edit the CMakeLists.txt file according to your installation path for compiler executable names and boost location.
You need the Boost library to be compiled for ARM as a dependency. You can compile a single threaded version to skip the pthread dependency. You will still require bzlib and zlib to be compiled for boost.
I'm using Sourcery CodeBench Lite 2011.09-70 4.6.1, but you most likely will be able to use other compilers.
You will also need mavlink installed so that the `mavgen.py` script can be used to generate the appropriate headers.

Compile
======

```
mkdir build
cd build
cmake ..
make
```

Send to drone
============

Connect to drone's wifi and do `make upload`. Executable ardrone-proxy will be saved on `/data/video`

Drone Modifications
==================

You need to comment out the ttyPA0 line on /etc/inittab. To do this, connect to the drone's wifi and then 
  `telnet 192.168.1.1`
Use vi to edit the file and reboot.

Running
======

Telnet into the drone, and execute ardrone-proxy. The process will be sent to background unless `-F` is passed as a parameter (mainly for debugging).

