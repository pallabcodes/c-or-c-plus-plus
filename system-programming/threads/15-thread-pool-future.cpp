#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
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
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
        using RetType = decltype(f(args...));
        auto task = std::make_shared<std::packaged_task<RetType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::future<RetType> res = task->get_future();
        {
            std::lock_guard<std::mutex> lock(mtx);
            tasks.push([task](){ (*task)(); });
        }
        cv.notify_one();
        return res;
    }
    ~ThreadPool() {
        stop = true;
        cv.notify_all();
        for (auto& t : workers) t.join();
    }
};

int main() {
    ThreadPool pool(4);
    auto fut1 = pool.enqueue([]{ return 42; });
    auto fut2 = pool.enqueue([](int x){ return x * 2; }, 21);
    std::cout << "fut1: " << fut1.get() << ", fut2: " << fut2.get() << std::endl;
    return 0;
}