cmake_minimum_required(VERSION 2.8)

SET(CMAKE_SYSTEM_NAME arm)
SET(CMAKE_SYSTEM_VERSION 1)

SET(CMAKE_C_COMPILER   arm-none-linux-gnueabi-gcc)
SET(CMAKE_CXX_COMPILER arm-none-linux-gnueabi-g++)
SET(CMAKE_LD_LINKER arm-none-linux-gnueabi-ld)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

set(BOOST_ROOT /usr/arm-none-linux-gnueabi/)
find_package(Boost COMPONENTS system REQUIRED)

include(mavlink.cmake)

include_directories(${PROJECT_SOURCE_DIR})
add_executable(ardrone-proxy ardrone-proxy.cpp serial.cpp network.cpp protocol.cpp)
add_dependencies(ardrone-proxy mavlink_headers)
target_link_libraries(ardrone-proxy boost_system)

add_custom_target(upload
  COMMAND curl -T ardrone-proxy ftp://192.168.1.1  
  DEPENDS ardrone-proxy
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pthread -Wfatal-errors")
