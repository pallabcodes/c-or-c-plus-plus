#include <tbb/concurrent_unordered_map.h>
#include <thread>
#include <iostream>

tbb::concurrent_unordered_map<int, int> cmap;

void insert(int key, int value) {
    cmap.insert({key, value});
}

void lookup(int key) {
    auto it = cmap.find(key);
    if (it != cmap.end())
        std::cout << "Found: " << key << " -> " << it->second << std::endl;
    else
        std::cout << "Not found: " << key << std::endl;
}

int main() {
    std::thread t1(insert, 1, 100), t2(insert, 2, 200);
    t1.join(); t2.join();
    std::thread t3(lookup, 1), t4(lookup, 3);
    t3.join(); t4.join();
    return 0;
}