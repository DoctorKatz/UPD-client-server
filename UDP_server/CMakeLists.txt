cmake_minimum_required(VERSION 3.17)
project(UDP_server)

set(CMAKE_CXX_STANDARD 20)

add_executable(UDP_server src/main.cpp inc/udp_server.h src/udp_server.cpp src/interaction.cpp inc/interaction.h src/lib.cpp inc/lib.h)