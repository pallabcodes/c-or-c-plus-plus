#include <atomic>
#include <thread>
#include <iostream>
#include <vector>

class LockFreeQueue {
    std::atomic<int> head{0}, tail{0};
    static const int SIZE = 1024;
    int data[SIZE];
public:
    bool enqueue(int val) {
        int t = tail.load(std::memory_order_relaxed);
        int h = head.load(std::memory_order_acquire);
        if ((t + 1) % SIZE == h) return false; // full
        data[t] = val;
        tail.store((t + 1) % SIZE, std::memory_order_release);
        return true;
    }
    bool dequeue(int &val) {
        int h = head.load(std::memory_order_relaxed);
        int t = tail.load(std::memory_order_acquire);
        if (h == t) return false; // empty
        val = data[h];
        head.store((h + 1) % SIZE, std::memory_order_release);
        return true;
    }
};

LockFreeQueue q;

void producer() {
    for (int i = 0; i < 1000; ++i) {
        while (!q.enqueue(i)) {}
    }
}

void consumer() {
    int val;
    int count = 0;
    while (count < 1000) {
        if (q.dequeue(val)) {
            ++count;
        }
    }
    std::cout << "Consumer received " << count << " items\n";
}

int main() {
    std::thread t1(producer), t2(consumer);
    t1.join(); t2.join();
    return 0;
}