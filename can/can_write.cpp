// A sample code of SocketCAN sender.
// Usage: ./can_send [vcan0] 123 DEADBEEF

#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdint>
#include <cstring>
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
void print_usage(char* argv[]) {
  std::cerr << "Usage: " << argv[0] << " [ CAN_DEVICE ] CAN_ID_HEX DATA_HEX" << std::endl;
}

#ifdef USE_CANFD
using CanFrameT = struct canfd_frame;
static constexpr size_t kMaxBytes = 64;
#else
using CanFrameT = struct can_frame;
static constexpr size_t kMaxBytes = 8;
#endif

/// The writer of CAN signals.
class CanWriter {
 public:
  /// Open a CAN socket and make it ready.
  explicit CanWriter(const std::string& ifname);
  /// Cleanup the socket if necessary.
  ~CanWriter();

  CanWriter& operator=(const CanWriter&) = delete;

  /// Write input data in the specified device.
  void Write(const std::string& id_hex, const std::string& data_hex);

 private:
  CanFrameT GenerateCanFrame(const std::string& id_hex, const std::string& data_hex) const;
  void CloseSocket();

  // File descriptor for the opened socket.
  std::optional<int> sock_ = std::nullopt;
};

CanWriter::CanWriter(const std::string& ifname) {
  // Open socket.
  sock_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
  if (sock_.value() < 0) {
    perror("socket");
    sock_.reset();
    return;
  }

#ifdef USE_CANFD
  // Enable CAN FD frames on this socket (FD build only).
  int enable_canfd = 1;
  if (setsockopt(sock_.value(), SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &enable_canfd,
                 sizeof(enable_canfd)) < 0) {
    perror("setsockopt(CAN_RAW_FD_FRAMES)");
    CloseSocket();
    return;
  }
#endif

  // Get the interface index.
  struct ifreq ifr {};
  std::strncpy(ifr.ifr_name, ifname.c_str(), IFNAMSIZ - 1);
  if (ioctl(sock_.value(), SIOCGIFINDEX, &ifr) < 0) {
    perror("ioctl(SIOCGIFINDEX)");
    CloseSocket();
    return;
  }

  // Bind.
  struct sockaddr_can addr {};
  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;
  if (bind(sock_.value(), reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
    perror("bind");
    CloseSocket();
    return;
  }
}

CanWriter::~CanWriter() { CloseSocket(); }

void CanWriter::Write(const std::string& id_hex, const std::string& data_hex) {
  if (!sock_.has_value()) {
    std::cerr << "Socket is not ready." << std::endl;
    return;
  }

  // Build frame from args.
  CanFrameT frame{};
  try {
    frame = GenerateCanFrame(id_hex, data_hex);
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    CloseSocket();
    return;
  }

  // Send.
  ssize_t n = write(sock_.value(), &frame, sizeof(frame));
  if (n < 0) {
    perror("write");
    CloseSocket();
    return;
  }

  // Optionally verify that all bytes were written.
  if (static_cast<size_t>(n) != sizeof(frame)) {
    std::cerr << "Partial write: " << n << " / " << sizeof(frame) << " bytes\n";
    CloseSocket();
    return;
  }
}

CanFrameT CanWriter::GenerateCanFrame(const std::string& id_hex,
                                      const std::string& data_hex) const {
  unsigned long id_ul = std::stoul(id_hex, nullptr, 16);
  if (id_ul > 0x1FFFFFFF) {
    throw std::runtime_error("CAN ID out of range (max 29-bit).");
  }
  const canid_t canid = static_cast<canid_t>(id_ul);

  // DATA_HEX length checks: even digits, within max bytes.
  if (data_hex.empty() || (data_hex.size() % 2) != 0) {
    throw std::runtime_error("Invalid DATA_HEX length (must be even number of hex digits).");
  }
  const size_t byte_len = data_hex.size() / 2;
  if (byte_len > kMaxBytes) {
    throw std::runtime_error(
        std::format("DATA_HEX too long for CAN FD (max {} bytes).", kMaxBytes));
  }

  CanFrameT frame{};
#ifdef USE_CANFD
  frame.can_id = (canid > 0x7FF) ? (canid | CAN_EFF_FLAG) : canid;
  frame.len = static_cast<__u8>(byte_len);
#else
  frame.can_id = (canid > 0x7FF) ? (canid | CAN_EFF_FLAG) : canid;
  frame.can_dlc = static_cast<__u8>(byte_len);
#endif

  // Convert data_hex to bytes.
  for (size_t i = 0; i < byte_len; ++i) {
    const std::string byte_hex = data_hex.substr(i * 2, 2);
    unsigned long v = std::stoul(byte_hex, nullptr, 16);
    if (v > 0xFFul) {
      throw std::runtime_error("Invalid hex byte in DATA_HEX: " + byte_hex);
    }
    frame.data[i] = static_cast<__u8>(v);
  }

  return frame;
}

void CanWriter::CloseSocket() {
  if (sock_.has_value()) {
    close(sock_.value());
    sock_.reset();
  }
}

}  // namespace

int main(int argc, char* argv[]) {
  // Parse arguments into the device name, CAN ID, data string.
  std::string ifname, id_hex, data_hex;
  if (argc == 3) {
    ifname = "vcan0";
    id_hex = argv[1];
    data_hex = argv[2];
  } else if (argc == 4) {
    ifname = argv[1];
    id_hex = argv[2];
    data_hex = argv[3];
  }

  if (ifname.empty() || id_hex.empty() || data_hex.empty()) {
    print_usage(argv);
    return 1;
  }

  CanWriter can_writer(ifname);
  can_writer.Write(id_hex, data_hex);
  return 0;
}
