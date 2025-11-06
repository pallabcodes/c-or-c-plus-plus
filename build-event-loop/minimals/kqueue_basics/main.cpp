#include <cstdio>
#include <cstdlib>

int main() {
#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
  int kq = ::kqueue();
  if (kq < 0) {
    std::perror("kqueue failed");
    return 1;
  }
  std::printf("kqueue created and closed\n");
  ::close(kq);
#else
  std::printf("kqueue not available on this platform\n");
#endif
  return 0;
}
