#define BOOST_TEST_MODULE AsyncSocketCanBoostTest
#include "async_socket_can.h"

#include <linux/can.h>
#include <linux/can/raw.h>
#include <unistd.h>

#include <atomic>
#include <boost/asio.hpp>
#include <boost/test/included/unit_test.hpp>
#include <cerrno>
#include <cstdint>
#include <functional>
#include <iostream>
#include <optional>

#include "can_message.h"

namespace {
ssize_t ReadSentData(int fd, void* buf, size_t n) {
  char* p = static_cast<char*>(buf);
  size_t offset = 0;
  while (offset < n) {
    ssize_t r = read(fd, p + offset, n - offset);
    if (r < 0) {
      if (errno == EINTR) continue;
      return -1;
    }
    if (r == 0) break;
    offset += static_cast<size_t>(r);
  }
  return static_cast<ssize_t>(offset);
}

ssize_t SendFakeData(int fd, const void* buf, size_t n) {
  const char* p = static_cast<const char*>(buf);
  size_t offset = 0;
  while (offset < n) {
    std::cout << "write data, " << offset << std::endl;
    ssize_t r = write(fd, p + offset, n - offset);
    if (r < 0) {
      if (errno == EINTR) continue;
      return -1;
    }
    offset += static_cast<size_t>(r);
  }
  return static_cast<ssize_t>(offset);
}
}  // namespace

// Send a classic can frame.
BOOST_AUTO_TEST_CASE(WriteSendClassicCanFrame) {
  int fds[2];
  pipe(fds);

  boost::asio::io_context io_context;
  constexpr bool kUseCanfd = false;
  can::AsyncSocketCan<can::CanMessageClassic> dut(io_context, fds[1], kUseCanfd);

  can::CanMessageClassic message = {.id = 0x123,
                                    .len = 3,
                                    .data = {0xAB, 0xCD, 0xEF},
                                    .type = can::CanMessageClassic::MessageType::kStandard};
  dut.Write(message);

  can_frame written_frame;
  BOOST_REQUIRE_EQUAL(ReadSentData(fds[0], &written_frame, sizeof(written_frame)),
                      static_cast<ssize_t>(sizeof(written_frame)));
  close(fds[0]);

  const auto expected_frame = std::get<can_frame>(can::ConvertMessage(message));
  BOOST_CHECK_EQUAL(written_frame.can_id, expected_frame.can_id);
  BOOST_CHECK_EQUAL(written_frame.can_dlc, expected_frame.can_dlc);
  for (int i = 0; i < message.len; ++i) {
    BOOST_CHECK_EQUAL(written_frame.data[i], expected_frame.data[i]);
  }
}

// Send a CANFD frame.
BOOST_AUTO_TEST_CASE(WriteSendCanFDFrame) {
  int fds[2];
  pipe(fds);

  boost::asio::io_context io_context;
  constexpr bool kUseCanfd = true;
  can::AsyncSocketCan<can::CanMessageFD> dut(io_context, fds[1], kUseCanfd);

  can::CanMessageFD message = {.id = 0x123456,
                               .len = 6,
                               .data = {0xAB, 0xCD, 0xEF, 0x01, 0x02, 0x03},
                               .type = can::CanMessageFD::MessageType::kStandard};
  dut.Write(message);

  canfd_frame written_frame{};
  BOOST_REQUIRE_EQUAL(ReadSentData(fds[0], &written_frame, sizeof(written_frame)),
                      static_cast<ssize_t>(sizeof(written_frame)));
  close(fds[0]);

  const auto expected_frame = std::get<canfd_frame>(can::ConvertMessage(message));
  BOOST_CHECK_EQUAL(written_frame.can_id, expected_frame.can_id);
  BOOST_CHECK_EQUAL(static_cast<int>(written_frame.len), message.len);
  for (int i = 0; i < message.len; ++i) {
    BOOST_CHECK_EQUAL(written_frame.data[i], message.data[i]);
  }
}

// Read a classic CAN frame.
BOOST_AUTO_TEST_CASE(AsyncReadClassicCanMessage) {
  int fds[2];
  pipe(fds);

  boost::asio::io_context io_context;
  constexpr bool kUseCanfd = false;
  can::AsyncSocketCan<can::CanMessageClassic> dut(io_context, fds[0], kUseCanfd);

  std::optional<can::CanMessageClassic> maybe_read_message = std::nullopt;
  dut.StartAsyncRead([&](const can::CanMessageClassic& m) { maybe_read_message = m; });

  can_frame frame;
  frame.can_id = 0x123 | CAN_SFF_MASK;
  frame.can_dlc = 3;
  frame.data[0] = 0x11;
  frame.data[1] = 0x22;
  frame.data[2] = 0x33;

  BOOST_REQUIRE_EQUAL(SendFakeData(fds[1], &frame, sizeof(frame)),
                      static_cast<ssize_t>(sizeof(frame)));
  close(fds[1]);

  for (int i = 0; i < 100 && !maybe_read_message.has_value(); ++i) {
    io_context.poll_one();
  }
  BOOST_REQUIRE(maybe_read_message.has_value());

  const auto& read_message = maybe_read_message.value();
  const auto expected_message = can::ConvertFrame<can::CanMessageClassic>(frame);
  BOOST_CHECK_EQUAL(read_message.id, expected_message.id);
#if 0
  BOOST_CHECK_EQUAL(read_message.type, can::CanMessageClassic::MessageType::kStandard);
#endif
  for (int i = 0; i < read_message.len; ++i) {
    BOOST_CHECK_EQUAL(read_message.data[i], frame.data[i]);
  }
}

// Read a CANFD frame.
BOOST_AUTO_TEST_CASE(AsyncReadCanFDMessage) {
  int fds[2];
  pipe(fds);

  boost::asio::io_context io_context;
  constexpr bool kUseCanfd = true;
  can::AsyncSocketCan<can::CanMessageFD> dut(io_context, fds[0], kUseCanfd);

  std::optional<can::CanMessageFD> maybe_read_message = std::nullopt;
  dut.StartAsyncRead([&](const can::CanMessageFD& m) { maybe_read_message = m; });

  canfd_frame frame;
  frame.can_id = 0x123456 | CAN_SFF_MASK;
  frame.len = 3;
  frame.data[0] = 0x1A;
  frame.data[1] = 0x2B;
  frame.data[2] = 0x3C;
  frame.data[3] = 0x4D;
  frame.data[4] = 0x5E;
  frame.data[5] = 0x6F;

  BOOST_REQUIRE_EQUAL(SendFakeData(fds[1], &frame, sizeof(frame)),
                      static_cast<ssize_t>(sizeof(frame)));
  close(fds[1]);

  for (int i = 0; i < 100 && !maybe_read_message.has_value(); ++i) {
    io_context.poll_one();
  }
  BOOST_REQUIRE(maybe_read_message.has_value());

  const auto& read_message = maybe_read_message.value();
  const auto expected_message = can::ConvertFrame<can::CanMessageFD>(frame);
  BOOST_CHECK_EQUAL(read_message.id, expected_message.id);
#if 0
  BOOST_CHECK_EQUAL(read_message.type, can::CanMessageClassic::MessageType::kStandard);
#endif
  for (int i = 0; i < read_message.len; ++i) {
    BOOST_CHECK_EQUAL(read_message.data[i], frame.data[i]);
  }
}
