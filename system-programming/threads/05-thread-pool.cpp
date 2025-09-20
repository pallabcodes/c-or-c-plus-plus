// Google-grade thread pool example (C++11)
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <atomic>

class ThreadPool {
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> stop{false};
public:
    ThreadPool(size_t n) {
        for (size_t i = 0; i < n; ++i)
            workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(mtx);
                        cv.wait(lock, [this]{ return stop || !tasks.empty(); });
                        if (stop && tasks.empty()) return;
                        task = std::move(tasks.front()); tasks.pop();
                    }
                    task();
                }
            });
    }
    void enqueue(std::function<void()> f) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            tasks.push(f);
        }
        cv.notify_one();
    }
    ~ThreadPool() {
        stop = true;
        cv.notify_all();
        for (auto& t : workers) t.join();
    }
};

int main() {
    ThreadPool pool(4);
    for (int i = 0; i < 8; ++i)
        pool.enqueue([i]{ std::cout << "Task " << i << " executed\n"; });
    // Destructor waits for all threads to finish
    return 0;
}