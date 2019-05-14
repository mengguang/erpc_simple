cmake_minimum_required(VERSION 3.14)
project(erpc_simple C)

set(CMAKE_C_STANDARD 99)

add_executable(erpc_simple main.c erpc.c erpc.h)
target_link_libraries(erpc_simple libserialport)