add_executable(async_socket_can_test
  async_socket_can_test.cpp
)
target_link_libraries(async_socket_can_test PRIVATE async_socket_can_lib)

add_test(NAME async_socket_can_test
         COMMAND async_socket_can_test)
