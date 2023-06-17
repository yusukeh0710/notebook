# boost::asio

link

https://www.boost.org/doc/libs/1_81_0/doc/html/boost_asio/tutorial/tuttimer1.html

### Setup / Compile

```
$ sudo apt install -y libboost-all-dev
$ g++ xxx.cpp -pthread
```



## Getting Started

```
#include <signal.h>

#include <iostream>
#include <random>
#include <thread>
#include <vector>

#include <boost/asio.hpp>

int rand_sleep() {
  int sleep_time = rand() % 5;
  sleep(sleep_time);
}

// Job.
void run(int no) {
  std::cout << "job " << no << " start" << std::endl;
  rand_sleep();
  std::cout << "job " << no << " end" << std::endl;
}

// Worker thread.
class Worker {
 public:
  explicit Worker(boost::asio::io_context& io, int no) : io_(io), no_(no) {}

  void Run() {
    while (!stop_) {
      io_.post(std::bind(run, no_));
      rand_sleep2();
    }
  }

  void Shutdown() { stop_ = true; }

 private:
  boost::asio::io_context& io_;
  int no_;
  bool stop_ = false;
};

int main() {
  boost::asio::io_context io;
  std::vector<Worker> workers;
  for (int i = 0; i < 3; ++i) {
    workers.emplace_back(io, i);
  }

  std::vector<std::thread> ths;
  for (auto& w : workers) {
    ths.emplace_back(&Worker::Run, &w);
  }

  // Process posted jobs while a que is not empty.
  io.run();

  // Since this progam has no trigger to finish loops,
  // please stop by Ctrl + C.
  for (auto& th : ths) {
    th.join();
  }
}
```

