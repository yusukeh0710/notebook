// A sample code of SocketCAN receiver.
// Usage: ./can_receive

#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <format>
#include <iostream>
#include <optional>
#include <string>

namespace {
#ifdef USE_CANFD
using CanFrameT = struct canfd_frame;
static constexpr size_t kMaxBytes = 64;
const std::string kMode = "CAN FD";
#else
using CanFrameT = struct can_frame;
static constexpr size_t kMaxBytes = 8;
const std::string kMode = "CAN";
#endif

/// The reader of written CAN signal.
class CanReader {
 public:
  /// Open a CAN socket and make it ready.
  explicit CanReader(const std::string& ifname);
  /// Cleanup the socket if necessary.
  ~CanReader();

  CanReader& operator=(const CanReader&) = delete;

  /// Start listening for CAN data.
  void Read();

 private:
  void CloseSocket();

  /// Target CAN device name.
  const std::string ifname_;

  // File descriptor for the opened socket.
  std::optional<int> sock_ = std::nullopt;
};

CanReader::CanReader(const std::string& ifname) : ifname_(ifname) {
  // Open RAW CAN socket.
  sock_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
  if (sock_.value() < 0) {
    perror("socket");
    sock_.reset();
    return;
  }

#ifdef USE_CANFD
  // Enable CAN FD frames when built with FD support.
  int enable_canfd = 1;
  if (setsockopt(sock.value(), SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &enable_canfd,
                 sizeof(enable_canfd)) < 0) {
    perror("setsockopt(CAN_RAW_FD_FRAMES)");
    CloseSocket();
    return;
  }
#endif

  // Resolve interface index.
  struct ifreq ifr {};
  std::strncpy(ifr.ifr_name, ifname_.c_str(), IFNAMSIZ - 1);
  if (ioctl(sock_.value(), SIOCGIFINDEX, &ifr) < 0) {
    perror("ioctl(SIOCGIFINDEX)");
    CloseSocket();
    return;
  }

  // 3. Bind to interface.
  struct sockaddr_can addr {};
  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;
  if (bind(sock_.value(), reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
    perror("bind");
    CloseSocket();
    return;
  }
}

CanReader::~CanReader() { CloseSocket(); }

void CanReader::Read() {
  if (!sock_.has_value()) {
    std::cerr << "Socket is not ready." << std::endl;
    return;
  }

#ifdef USE_CANFD
  std::cout << "Listening (CAN FD) on " << ifname_ << " ..." << std::endl;
#else
  std::cout << "Listening on " << ifname_ << " ..." << std::endl;
#endif

  while (true) {
    CanFrameT frame;
    int nbytes = read(sock_.value(), &frame, sizeof(frame));
    if (nbytes < 0) {
      perror("read");
      break;
    }

#ifdef USE_CANFD
    std::cout << "CAN ID: 0x" << std::hex << frame.can_id << " LEN: " << std::dec
              << static_cast<int>(frame.len) << " Data:";
    for (int i = 0; i < frame.len; ++i) {
      std::cout << " " << std::hex << static_cast<int>(frame.data[i]);
    }
#else
    std::cout << "CAN ID: 0x" << std::hex << frame.can_id << " DLC: " << std::dec
              << static_cast<int>(frame.can_dlc) << " Data:";
    for (int i = 0; i < frame.can_dlc; ++i) {
      std::cout << " " << std::hex << static_cast<int>(frame.data[i]);
    }
#endif
    std::cout << std::dec << std::endl;
  }
}

void CanReader::CloseSocket() {
  if (sock_.has_value()) {
    close(sock_.value());
    sock_.reset();
  }
}
}  // namespace

int main(int argc, char* argv[]) {
  std::string ifname = "vcan0";
  if (argc > 1) {
    ifname = argv[1];
  }

  CanReader can_communicator(ifname);
  can_communicator.Read();
  return 0;
}
