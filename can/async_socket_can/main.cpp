#include <stdio.h>

#include <atomic>
#include <boost/asio.hpp>
#include <cstdint>
#include <iostream>
#include <string>
#include <thread>

#include "async_socket_can.h"
#include "can_message.h"

namespace {
std::atomic<int> g_running = false;
void HandleSigInt(int) { g_running = false; }
}  // namespace

int main(int argc, char** argv) {
  std::string ifname = (argc > 1) ? argv[1] : "vcan0";

  std::atomic<bool> running = false;
  std::signal(SIGINT, HandleSigInt);

  boost::asio::io_context io_context;
  can::AsyncSocketCan<can::CanMessageFD> socket_can(io_context, ifname);
  socket_can.StartAsyncRead([](const can::CanMessageFD& message) {
    printf("rx id=0x%03X len=%d type=%d: ", message.id, message.len,
           static_cast<int>(message.type));
    for (int i = 0; i < message.len; ++i) {
      printf("%02x ", message.data[i]);
    }
    printf("\n");
    fflush(stdout);
  });
  std::thread main_thread([&io_context] { io_context.run(); });

  uint8_t v = 0;
  g_running = true;
  while (g_running) {
    std::cout << "Send CAN message" << std::endl;
    can::CanMessageFD message{
        .id = 0x123,
        .len = 6,
        .type = can::CanMessageFD::MessageType::kStandard,
    };
    std::fill_n(message.data.begin(), message.len, static_cast<uint8_t>(v));
    socket_can.AsyncWrite(message);

    v = (v + 1) % 256;
    sleep(10);
  }

  std::cout << "Terminated. Waiting for closing threads." << std::endl;
  io_context.stop();
  if (main_thread.joinable()) {
    main_thread.join();
  }
  return 0;
}
