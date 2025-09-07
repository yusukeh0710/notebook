#pragma once

#include <linux/can.h>
#include <linux/can/raw.h>

#include <array>
#include <boost/asio.hpp>
#include <boost/core/noncopyable.hpp>
#include <functional>
#include <string>
#include <variant>

#include "can_message.h"

namespace can {
template <typename CanMessageT>
class AsyncSocketCan : private boost::noncopyable {
 public:
  explicit AsyncSocketCan(boost::asio::io_context& io_context, const std::string& device_name,
                          bool use_canfd = true);

  // The interface for unit tests.
  explicit AsyncSocketCan(boost::asio::io_context& io_context, int fd, bool use_canfd = true);

  void StartAsyncRead(std::function<void(const CanMessageT&)> callback);
  void Stop();
  void AsyncWrite(const CanMessageT& msg);

 private:
  void AsyncRead();

  boost::asio::io_context& io_context_;
  boost::asio::posix::stream_descriptor socket_;

  std::function<void(const CanMessageT&)> callback_;
  std::variant<can_frame, canfd_frame> read_frame_;
  std::variant<can_frame, canfd_frame> write_frame_;
};
}  // namespace can
