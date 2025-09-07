cmake_minimum_required(VERSION 3.14)
project(async_socket_can LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)
find_package(Boost 1.70 QUIET COMPONENTS system)

add_library(async_socket_can_lib STATIC
  can_message.cpp
  async_socket_can.cpp
)
target_include_directories(async_socket_can_lib PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(async_socket_can_lib PUBLIC
  Threads::Threads
  Boost::system
)

add_executable(async_socket_can
  main.cpp
)
target_link_libraries(async_socket_can PRIVATE async_socket_can_lib)

enable_testing()
include(CTest)
if(BUILD_TESTING)
  add_subdirectory(test)
endif()
