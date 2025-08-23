#include <vector>
#include <thread>
#include <iostream>
#include <numeric>

void partial_sum(const std::vector<int>& data, int start, int end, int& result) {
    result = std::accumulate(data.begin() + start, data.begin() + end, 0);
}

int main() {
    std::vector<int> data(10000, 1);
    int mid = data.size() / 2;
    int sum1 = 0, sum2 = 0;
    std::thread t1(partial_sum, std::ref(data), 0, mid, std::ref(sum1));
    std::thread t2(partial_sum, std::ref(data), mid, data.size(), std::ref(sum2));
    t1.join(); t2.join();
    std::cout << "Parallel sum: " << sum1 + sum2 << std::endl;
    return 0;
}