#include "async_socket_can.h"

#include <gtest/gtest.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <unistd.h>

#include <boost/asio.hpp>
#include <cstdint>
#include <optional>

#include "can_message.h"

TEST(AddTest, AsyncReadClassicCanMessage) {
  int fds[2];
  pipe(fds);

  boost::asio::io_context io_context;
  constexpr bool kUseCanfd = false;
  can::AsyncSocketCan<can::CanMessageClassic> dut(io_context, fds[0], kUseCanfd);

  std::optional<can::CanMessageClassic> maybe_read_message = std::nullopt;
  dut.StartAsyncRead([&](const can::CanMessageClassic& m) { maybe_read_message = m; });
  io_context.poll();

  can_frame frame;
  frame.can_id = 0x123 | CAN_SFF_MASK;
  frame.can_dlc = 3;
  frame.data[0] = 0x11;
  frame.data[1] = 0x22;
  frame.data[2] = 0x33;

  ssize_t data_size = write(fds[1], &frame, sizeof(frame));
  ASSERT_EQ(data_size, sizeof(frame));
  close(fds[1]);

  io_context.poll_one();
  ASSERT_TRUE(maybe_read_message.has_value());

  const auto& read_message = maybe_read_message.value();
  const auto expected_message = can::ConvertFrame<can::CanMessageClassic>(frame);
  EXPECT_EQ(read_message.id, expected_message.id);
  EXPECT_EQ(read_message.type, can::CanMessageClassic::MessageType::kStandard);
  for (int i = 0; i < read_message.len; ++i) {
    EXPECT_EQ(read_message.data[i], frame.data[i]);
  }
}

TEST(AddTest, AsyncReadCanMessageFD) {
  int fds[2];
  pipe(fds);

  boost::asio::io_context io_context;
  constexpr bool kUseCanfd = true;
  can::AsyncSocketCan<can::CanMessageFD> dut(io_context, fds[0], kUseCanfd);

  std::optional<can::CanMessageFD> maybe_read_message = std::nullopt;
  dut.StartAsyncRead([&](const can::CanMessageFD& m) { maybe_read_message = m; });
  io_context.poll();

  canfd_frame frame;
  frame.can_id = 0x123 | CAN_SFF_MASK;
  frame.len = 3;
  frame.data[0] = 0x11;
  frame.data[1] = 0x22;
  frame.data[2] = 0x33;

  ssize_t data_size = write(fds[1], &frame, sizeof(frame));
  ASSERT_EQ(data_size, sizeof(frame));
  close(fds[1]);

  io_context.poll_one();
  ASSERT_TRUE(maybe_read_message.has_value());

  const auto& read_message = maybe_read_message.value();
  const auto expected_message = can::ConvertFrame<can::CanMessageFD>(frame);
  EXPECT_EQ(read_message.id, expected_message.id);
  EXPECT_EQ(read_message.type, can::CanMessageFD::MessageType::kStandard);
  for (int i = 0; i < read_message.len; ++i) {
    EXPECT_EQ(read_message.data[i], frame.data[i]);
  }
}

TEST(AddTest, AsyncWriteCanFrame) {
  int fds[2];
  pipe(fds);

  boost::asio::io_context io_context;
  constexpr bool kUseCanfd = false;
  can::AsyncSocketCan<can::CanMessageClassic> dut(io_context, fds[1], kUseCanfd);

  can::CanMessageClassic message = {.id = 0x123,
                                    .len = 3,
                                    .data = {0xAB, 0xCD, 0xEF},
                                    .type = can::CanMessageClassic::MessageType::kStandard};

  io_context.poll();
  dut.AsyncWrite(message);

  io_context.poll_one();
  can_frame written_frame;
  ssize_t data_size = read(fds[0], &written_frame, sizeof(written_frame));
  ASSERT_EQ(data_size, sizeof(written_frame));
  close(fds[0]);

  const auto expected_frame = std::get<can_frame>(can::ConvertMessage(message));
  EXPECT_EQ(written_frame.can_id, expected_frame.can_id);
  EXPECT_EQ(written_frame.can_dlc, expected_frame.can_dlc);
  for (int i = 0; i < message.len; ++i) {
    EXPECT_EQ(written_frame.data[i], expected_frame.data[i]);
  }
}

TEST(AddTest, AsyncWriteCanFDFrame) {
  int fds[2];
  pipe(fds);

  boost::asio::io_context io_context;
  constexpr bool kUseCanfd = false;
  can::AsyncSocketCan<can::CanMessageFD> dut(io_context, fds[1], kUseCanfd);

  can::CanMessageFD message = {.id = 0x123,
                               .len = 3,
                               .data = {0xAB, 0xCD, 0xEF},
                               .type = can::CanMessageFD::MessageType::kStandard};

  io_context.poll();
  dut.AsyncWrite(message);

  io_context.poll_one();
  canfd_frame written_frame;
  ssize_t data_size = read(fds[0], &written_frame, sizeof(written_frame));
  ASSERT_EQ(data_size, sizeof(written_frame));
  close(fds[0]);

  const auto expected_frame = std::get<canfd_frame>(can::ConvertMessage(message));
  EXPECT_EQ(written_frame.can_id, expected_frame.can_id);
  EXPECT_EQ(written_frame.len, expected_frame.len);
  for (int i = 0; i < message.len; ++i) {
    EXPECT_EQ(written_frame.data[i], expected_frame.data[i]);
  }
}
