cmake_minimum_required(VERSION 3.10)
project(SocketCANReceiver CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(USE_CANFD "Enable CAN FD support" OFF)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if(USE_CANFD)
  add_definitions(-DUSE_CANFD)
endif()

add_executable(can_read can_read.cpp)
add_executable(can_write can_write.cpp)
