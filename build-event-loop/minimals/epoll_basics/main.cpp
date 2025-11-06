#include <cstdio>
#include <cstdlib>
#include <cstring>

int main() {
#if defined(__linux__)
  // Minimal epoll setup and immediate teardown without real sockets
  int fd = ::epoll_create1(0);
  if (fd < 0) {
    std::perror("epoll_create1 failed");
    return 1;
  }
  std::printf("epoll created and closed\n");
  ::close(fd);
#else
  std::printf("epoll not available on this platform\n");
#endif
  return 0;
}
