#include <unordered_map>
#include <mutex>
#include <thread>
#include <iostream>

std::unordered_map<int, int> cmap;
std::mutex cmap_mutex;

void insert(int key, int value) {
    std::lock_guard<std::mutex> lock(cmap_mutex);
    cmap[key] = value;
}

void lookup(int key) {
    std::lock_guard<std::mutex> lock(cmap_mutex);
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