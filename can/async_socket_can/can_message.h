#pragma once

#include <linux/can.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <variant>

namespace can {

template <std::size_t L>
struct CanMessageBase {
  uint32_t id = 0;
  int len = 0;
  std::array<uint8_t, L> data{};

  enum class MessageType : uint8_t {
    kStandard = 0,
    kExtended = 1,
    kRemote = 2,
    kError = 3,
  };
  MessageType type = MessageType::kStandard;
};

using CanMessageClassic = CanMessageBase<8>;
using CanMessageFD = CanMessageBase<64>;

template <typename CanMessageT>
CanMessageT ConvertFrame(const std::variant<can_frame, canfd_frame>& frame);

template <typename CanMessageT>
std::variant<can_frame, canfd_frame> ConvertMessage(const CanMessageT& message);
}  // namespace can
