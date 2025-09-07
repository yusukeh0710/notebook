#include "async_socket_can.h"

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>

namespace can {
namespace {
void HandleError(const boost::system::error_code& ec) {
  if (ec) {
    std::cerr << ec.message() << std::endl;
  }
}
}  // namespace

template <typename CanMessageT>
AsyncSocketCan<CanMessageT>::AsyncSocketCan(boost::asio::io_context& io_context,
                                            const std::string& device_name, bool use_canfd)
    : io_context_(io_context), socket_(io_context) {
  // Open RAW CAN socket.
  auto can_socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
  if (can_socket < 0) {
    throw std::system_error(errno, std::generic_category(), "Socket open failed.");
  }

  // Enable CAN FD frames when built with FD support.
  if (use_canfd) {
    int enable_canfd = 1;
    if (setsockopt(can_socket, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &enable_canfd,
                   sizeof(enable_canfd)) < 0) {
      close(can_socket);
      throw std::system_error(errno, std::generic_category(), "Enabling CANFD failed.");
    }
  }

  // Resolve interface index.
  struct ifreq ifr {};
  std::strncpy(ifr.ifr_name, device_name.c_str(), IFNAMSIZ - 1);
  if (ioctl(can_socket, SIOCGIFINDEX, &ifr) < 0) {
    close(can_socket);
    throw std::system_error(errno, std::generic_category(), "SIOCGIFINDEX failed.");
  }

  // 3. Bind to interface.
  struct sockaddr_can addr {};
  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;
  if (bind(can_socket, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
    close(can_socket);
    throw std::system_error(errno, std::generic_category(), "biding socket to device failed.");
  }
  socket_.assign(can_socket);

  if (use_canfd) {
    read_frame_ = can_frame();
    write_frame_ = can_frame();
  } else {
    read_frame_ = canfd_frame();
    write_frame_ = canfd_frame();
  }
}

template <typename CanMessageT>
AsyncSocketCan<CanMessageT>::AsyncSocketCan(boost::asio::io_context& io_context, int fd,
                                            bool use_canfd)
    : io_context_(io_context), socket_(io_context) {
  assert(fd >= 0);
  socket_.assign(fd);

  if (use_canfd) {
    read_frame_ = can_frame();
    write_frame_ = can_frame();
  } else {
    read_frame_ = canfd_frame();
    write_frame_ = canfd_frame();
  }
}

template <typename CanMessageT>
void AsyncSocketCan<CanMessageT>::StartAsyncRead(std::function<void(const CanMessageT&)> callback) {
  if (!socket_.is_open()) {
    std::cerr << "Socket is not opened." << std::endl;
    return;
  }
  callback_ = std::move(callback);
  AsyncRead();
}

template <typename CanMessageT>
void AsyncSocketCan<CanMessageT>::Stop() {
  boost::system::error_code ec;
  socket_.cancel(ec);
  socket_.close(ec);
}

template <typename CanMessageT>
void AsyncSocketCan<CanMessageT>::AsyncWrite(const CanMessageT& message) {
  write_frame_ = ConvertMessage(message);
  std::visit(
      [this](auto& frame) {
        socket_.async_write_some(boost::asio::buffer(&frame, sizeof(frame)),
                                 [this](const boost::system::error_code& ec, std::size_t) {
                                   if (ec) {
                                     HandleError(ec);
                                   }
                                 });
      },
      write_frame_);
}

template <typename CanMessageT>
void AsyncSocketCan<CanMessageT>::AsyncRead() {
  std::visit(
      [this](auto& frame) {
        socket_.async_read_some(boost::asio::buffer(&frame, sizeof(frame)),
                                [this](const boost::system::error_code& ec, std::size_t) {
                                  if (ec) {
                                    HandleError(ec);
                                    return;
                                  }
                                  const auto message = ConvertFrame<CanMessageT>(read_frame_);
                                  if (callback_) callback_(message);
                                  AsyncRead();
                                });
      },
      read_frame_);
}

template class AsyncSocketCan<CanMessageClassic>;
template class AsyncSocketCan<CanMessageFD>;
}  // namespace can
