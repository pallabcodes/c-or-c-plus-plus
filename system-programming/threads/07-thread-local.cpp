#include <thread>
#include <iostream>
thread_local int tls_var = 0;
void f(int id) {
    tls_var = id;
    std::cout << "Thread " << id << " tls_var: " << tls_var << std::endl;
}
int main() {
    std::thread t1(f, 1), t2(f, 2);
    t1.join(); t2.join();
    return 0;
}