#include <chrono>
#include <cstdio>
#include <queue>
#include <thread>
#include <vector>

struct Timer {
  std::chrono::steady_clock::time_point when;
  int id;
};
struct Cmp {
  bool operator()(const Timer& a, const Timer& b) const {
    return a.when > b.when;
  }
};

int main() {
  std::priority_queue<Timer, std::vector<Timer>, Cmp> pq;
  auto now = std::chrono::steady_clock::now();
  pq.push({now + std::chrono::milliseconds(100), 1});
  pq.push({now + std::chrono::milliseconds(10), 2});

  while (!pq.empty()) {
    auto t = pq.top();
    auto now2 = std::chrono::steady_clock::now();
    if (now2 < t.when) {
      std::this_thread::sleep_for(t.when - now2);
    }
    std::printf("timer %d fired\n", t.id);
    pq.pop();
  }
  return 0;
}
