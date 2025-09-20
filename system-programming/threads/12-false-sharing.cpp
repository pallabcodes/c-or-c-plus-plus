#include <thread>
#include <iostream>
#include <atomic>

struct alignas(64) PaddedInt {
    std::atomic<int> value;
    char pad[64 - sizeof(std::atomic<int>)];
};

PaddedInt arr[2];

void inc(int idx) {
    for (int i = 0; i < 1000000; ++i)
        arr[idx].value++;
}

int main() {
    std::thread t1(inc, 0), t2(inc, 1);
    t1.join(); t2.join();
    std::cout << "arr[0]: " << arr[0].value << ", arr[1]: " << arr[1].value << std::endl;
    return 0;
}