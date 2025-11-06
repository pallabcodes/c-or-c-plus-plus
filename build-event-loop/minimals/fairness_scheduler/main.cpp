#include <cstdio>
#include <queue>
#include <vector>

int main() {
  std::vector<std::queue<int>> qs(3);
  for (int i = 0; i < 5; ++i) qs[0].push(100 + i);
  for (int i = 0; i < 2; ++i) qs[1].push(200 + i);
  for (int i = 0; i < 4; ++i) qs[2].push(300 + i);
  size_t idx = 0;
  int served = 0;
  while (served < 11) {
    if (!qs[idx].empty()) {
      std::printf("serve %d from q%zu\n", qs[idx].front(), idx);
      qs[idx].pop();
      ++served;
    }
    idx = (idx + 1) % qs.size();
  }
  return 0;
}
