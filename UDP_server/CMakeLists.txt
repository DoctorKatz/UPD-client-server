cmake_minimum_required(VERSION 3.17)
project(UDP_server)

set(CMAKE_CXX_STANDARD 20)

add_executable(UDP_server main.cpp udp_server.h udp_server.cpp interaction.cpp interaction.h ../UDP_client/src/crc32c.c)