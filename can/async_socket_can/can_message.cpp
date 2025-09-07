#include "can_message.h"

#include <algorithm>
#include <cassert>
#include <type_traits>

namespace can {

namespace {
inline uint32_t ParseCanFrameId(uint32_t canid) {
  if (canid & CAN_ERR_FLAG) {
    return canid & CAN_ERR_MASK;
  } else if (canid & CAN_EFF_FLAG) {
    return canid & CAN_EFF_MASK;
  } else if (canid & CAN_RTR_FLAG) {
    return canid & CAN_SFF_MASK;
  } else {
    return canid & CAN_SFF_MASK;
  }
}

template <typename CanMessageT>
typename CanMessageT::MessageType ParseCanFrameType(uint32_t canid) {
  if (canid & CAN_ERR_FLAG) {
    return CanMessageT::MessageType::kError;
  } else if (canid & CAN_EFF_FLAG) {
    return CanMessageT::MessageType::kExtended;
  } else if (canid & CAN_RTR_FLAG) {
    return CanMessageT::MessageType::kRemote;
  } else {
    return CanMessageT::MessageType::kStandard;
  }
}

template <typename CanMessageT>
uint32_t ParseCanMessageId(uint32_t id, typename CanMessageT::MessageType type) {
  switch (type) {
    case CanMessageT::MessageType::kError:
      return (id & CAN_ERR_MASK) | CAN_ERR_FLAG;
    case CanMessageT::MessageType::kExtended:
      return (id & CAN_EFF_MASK) | CAN_EFF_FLAG;
    case CanMessageT::MessageType::kRemote:
      return (id & CAN_SFF_MASK) | CAN_RTR_FLAG;
    case CanMessageT::MessageType::kStandard:
      return (id & CAN_SFF_MASK);
    default:
      assert(false);
  }
}

}  // namespace

template <typename CanMessageT>
CanMessageT ConvertFrame(const std::variant<can_frame, canfd_frame>& frame) {
  CanMessageT message;
  if (std::holds_alternative<can_frame>(frame)) {
    const auto& frame_data = std::get<can_frame>(frame);
    message.id = ParseCanFrameId(frame_data.can_id);
    message.type = ParseCanFrameType<CanMessageT>(frame_data.can_id);
    message.len = static_cast<int>(frame_data.can_dlc);

    assert(frame_data.can_dlc <= message.data.size());
    std::copy_n(frame_data.data, frame_data.can_dlc, message.data.begin());
  } else if (std::holds_alternative<canfd_frame>(frame)) {
    const auto& frame_data = std::get<canfd_frame>(frame);
    message.id = ParseCanFrameId(frame_data.can_id);
    message.type = ParseCanFrameType<CanMessageT>(frame_data.can_id);
    message.len = static_cast<int>(frame_data.len);

    assert(frame_data.len <= message.data.size());
    std::copy_n(frame_data.data, frame_data.len, message.data.begin());
  }
  return message;
}

template <typename CanMessageT>
std::variant<can_frame, canfd_frame> ConvertMessage(const CanMessageT& message) {
  if constexpr (std::is_same_v<CanMessageT, CanMessageClassic>) {
    can_frame frame;
    frame.can_id = ParseCanMessageId<CanMessageT>(message.id, message.type);
    frame.can_dlc = static_cast<size_t>(message.len);
    std::copy_n(message.data.data(), message.len, frame.data);
    return frame;
  } else {
    canfd_frame frame;
    frame.can_id = ParseCanMessageId<CanMessageT>(message.id, message.type);
    frame.len = static_cast<size_t>(message.len);
    std::copy_n(message.data.data(), message.len, frame.data);
    return frame;
  }
}

template CanMessageClassic ConvertFrame<CanMessageClassic>(
    const std::variant<can_frame, canfd_frame>& frame);
template CanMessageFD ConvertFrame<CanMessageFD>(const std::variant<can_frame, canfd_frame>& frame);

template std::variant<can_frame, canfd_frame> ConvertMessage(const CanMessageClassic& message);
template std::variant<can_frame, canfd_frame> ConvertMessage(const CanMessageFD& message);
}  // namespace can
