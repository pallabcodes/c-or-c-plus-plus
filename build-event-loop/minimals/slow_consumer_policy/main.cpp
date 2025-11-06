#include <cstdio>
#include <deque>

int main() {
  std::deque<int> q;
  const size_t cap = 4;
  auto enqueue = [&](int v) {
    if (q.size() >= cap) {
      std::printf("slow consumer: drop oldest %d to make room for %d\n", q.front(), v);
      q.pop_front();
    }
    q.push_back(v);
  };
  for (int i = 0; i < 8; ++i) enqueue(i);
  while (!q.empty()) {
    std::printf("deliver %d\n", q.front());
    q.pop_front();
  }
  return 0;
}
