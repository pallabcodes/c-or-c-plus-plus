#include <mutex>
#include <thread>
#include <iostream>
std::mutex log_mutex;
void log(const std::string& msg) {
    std::lock_guard<std::mutex> lock(log_mutex);
    std::cout << msg << std::endl;
}
void worker(int id) {
    log("Thread " + std::to_string(id) + " logging safely");
}
int main() {
    std::thread t1(worker, 1), t2(worker, 2);
    t1.join(); t2.join();
    return 0;
}