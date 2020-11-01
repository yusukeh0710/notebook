# Socket connection

### Get started

server.cc (receiver)

```c++
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>

constexpr int kPortNo = 1234;
constexpr int kBufferSize = 1024;

int main() {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    std::cerr << "Error: socket creation" << std::endl;
    return 1;
  }

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(kPortNo);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    std::cerr << "Error: bind" << std::endl;
    return 1;
  }

  if (listen(sockfd, SOMAXCONN) < 0) {
    std::cerr << "Error: listen" << std::endl;
    return 1;
  }

  int client_sockfd;
  struct sockaddr_in from_addr;
  socklen_t len = sizeof(struct sockaddr_in);
  if ((client_sockfd = accept(sockfd, (struct sockaddr*)&from_addr, &len)) < 0) {
    std::cerr << "Error: accept" << std::endl;
    return 1;
  }

  // Main loop (Receiving)
  char buffer[kBufferSize];
  memset(buffer, 0, sizeof(buffer));
  while (true) {
    int data_size = recv(client_sockfd, buffer, sizeof(buffer), 0);
    if (data_size == 0) {
      break;
    } else if (data_size < 0) {
      std::cerr << "Error: recv" << std::endl;
      return 1;
    } else {
      std::cout << "Receive: " << buffer << std::endl;
    }
  }

  // Close.
  close(client_sockfd);
  close(sockfd);
  return 0;
}
```



client.cc (transmitter)

```c++
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

constexpr int kPortNo = 1234;
constexpr int kDataSize = 10;

int main() {
  // Create socket.
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    std::cerr << "Error: socket creation" << std::endl;
    return 1;
  }

  // Set destination address/portno.
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(kPortNo);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  // Connect.
  connect(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));

  // Main loop (Sending)
  while (true) {
    std::cout << "input: ";
    std::string data; std::cin >> data;
    if (data == "end") break;
    if (send(sockfd, data.c_str(), kDataSize, 0) < 0) {
      std::cerr << "Error: send" << std::endl;
      return 0;
    }
  }

  // Close.
  close(sockfd);
  return 0;
}
```



## Detail

```c++
int socket(int protocolFamily, int type, int protocol)
```

protocolFamily

- PF_UNIX, PF_LOCAL: Local connection in the machine
- PF_INET, PF_INET6: IPv4 / v6 connection
- PF_PACKET: Connection via L2 layer (data-link layer)
- PF_IPX, PF_APPLETALK:  IPX, AppleTalk

type

- SOCK_STREAM: TCP
- SOCK_DGRAM: UDP

protocol

- 0: Selected appropriate protocol
- IPPROTO_TCP, IPPROTO_UDP: connection via TCP/UDP

return_code: file descriptor of created socket



```c++
struct sockaddr_in addr;
```

- addr.sin_family: 
  AF_UNIX, AF_LOCAL, AF_INET, AF_INET6, AF_PACKET, AF_IPX, AF_APPLETALK
- addr.sin_port: port no
- addr.sin_addr.s_addr: source address



```c++
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

- sockfd:  socket file descriptor
- addr: address data
- addrlen: the size of addr
- return_code: success -> 0, error -> -1



```c++
int listen(int sockfd, int backlog);
```

- sockfd: socket file descriptor
- backlog: 
  - max queue size
  - SOMAXCONN: default 128, it is loaded from sysctl.conf (net.core.somaxconn)
- return_code: success -> 0, error -> -1



```c++
int accept(int sockfd, struct sockaddr *addr, &sizeof(struct sockaddr_in));
```

```c++
int connect(int sockfd, struct sockaddr *addr, sizeof(struct sockaddr_in));
```

- return_code: 
  - accept: success -> client socket file descriptor, error->-1
  - connect: success -> 0, error -> -1



```c++
ssize_t recv(int sockfd, void *buf, size_t sizeof(buf), int flags);
```

```c++
size_t send(int sockfd, const void *buf, size_t sizeof(buf), int flags);
```

- flags
  - 0: no flag
  - recv flag: 
    - define non-blocking mode, all wait mode, ..etc
    - https://linuxjm.osdn.jp/html/LDP_man-pages/man2/recv.2.html
  - send flag
    - https://linuxjm.osdn.jp/html/LDP_man-pages/man2/send.2.html
- return_code: data size which it receives/sends



```c++
int close(int fd)
```

- return_code: success -> 0, error -> -1