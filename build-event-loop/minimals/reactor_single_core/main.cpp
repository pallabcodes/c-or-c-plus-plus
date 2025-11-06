#include <cstdio>
#include <vector>

int main() {
#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
  int kq = ::kqueue();
  if (kq < 0) {
    std::perror("kqueue failed");
    return 1;
  }
  std::printf("single core reactor using kqueue\n");
  ::close(kq);
#elif defined(__linux__)
  int ep = ::epoll_create1(0);
  if (ep < 0) {
    std::perror("epoll_create1 failed");
    return 1;
  }
  std::printf("single core reactor using epoll\n");
  ::close(ep);
#else
  std::printf("no platform reactor available\n");
#endif
  return 0;
}
