include_directories(${GTEST_INCLUDE_DIRS})
find_package(GTest REQUIRED)

add_executable(async_socket_can_test
  async_socket_can_test.cpp
)
target_link_libraries(async_socket_can_test
  PRIVATE
  async_socket_can_lib
  GTest::GTest
  GTest::Main
  pthread
)

#add_test(NAME async_socket_can_test
#         COMMAND async_socket_can_test)
include(GoogleTest)
gtest_discover_tests(async_socket_can_test)
