#include <cstdio>
#include <queue>

int main() {
  const size_t high = 4;
  const size_t low = 2;
  std::queue<int> q;
  auto push = [&](int v) {
    if (q.size() >= high) {
      std::printf("backpressure: drop %d at high watermark\n", v);
      return false;
    }
    q.push(v);
    return true;
  };
  for (int i = 0; i < 8; ++i) push(i);
  while (!q.empty()) {
    std::printf("consume %d\n", q.front());
    q.pop();
    if (q.size() == low) std::printf("watermark low reached\n");
  }
  return 0;
}
