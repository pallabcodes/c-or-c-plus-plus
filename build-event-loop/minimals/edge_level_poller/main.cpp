#include <cstdio>

int main() {
#if defined(__linux__)
  std::printf("edge vs level: epoll supports both; choose edge with drain loops\n");
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
  std::printf("edge vs level: kqueue supports EV_CLEAR (edge like) and level behavior\n");
#else
  std::printf("no advanced poller; concept only\n");
#endif
  return 0;
}
