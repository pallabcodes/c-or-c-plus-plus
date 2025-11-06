#include <atomic>
#include <cstdio>
#include <vector>

struct Spsc {
  std::vector<int> buf;
  const size_t cap;
  std::atomic<size_t> head{0};
  std::atomic<size_t> tail{0};
  Spsc(size_t c) : buf(c), cap(c) {}
  bool push(int v) {
    auto t = tail.load(std::memory_order_relaxed);
    auto h = head.load(std::memory_order_acquire);
    if (((t + 1) % cap) == h) return false;
    buf[t] = v;
    tail.store((t + 1) % cap, std::memory_order_release);
    return true;
  }
  bool pop(int& out) {
    auto h = head.load(std::memory_order_relaxed);
    auto t = tail.load(std::memory_order_acquire);
    if (h == t) return false;
    out = buf[h];
    head.store((h + 1) % cap, std::memory_order_release);
    return true;
  }
};

int main() {
  Spsc q(8);
  for (int i = 0; i < 5; ++i) q.push(i);
  int x;
  while (q.pop(x)) std::printf("%d ", x);
  std::printf("\n");
  return 0;
}
