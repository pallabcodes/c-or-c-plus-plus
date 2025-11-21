#include <iostream>
#include <atomic>
#include <thread>
#include <vector>

using namespace std;

// Lock-free stack using compare-and-swap (CAS)
// Thread-safe without mutexes
template<typename T>
class LockFreeStack {
private:
    struct Node {
        T data;
        Node* next;

        Node(const T& d) : data(d), next(nullptr) {}
    };

    atomic<Node*> head;

public:
    LockFreeStack() : head(nullptr) {}

    void push(const T& data) {
        Node* newNode = new Node(data);
        Node* oldHead = head.load();
        
        do {
            newNode->next = oldHead;
        } while (!head.compare_exchange_weak(oldHead, newNode));
    }

    bool pop(T& result) {
        Node* oldHead = head.load();
        
        do {
            if (oldHead == nullptr) {
                return false;
            }
        } while (!head.compare_exchange_weak(oldHead, oldHead->next));

        result = oldHead->data;
        delete oldHead;
        return true;
    }

    bool isEmpty() const {
        return head.load() == nullptr;
    }
};

// Lock-free queue using two CAS operations
template<typename T>
class LockFreeQueue {
private:
    struct Node {
        atomic<T*> data;
        atomic<Node*> next;

        Node() : data(nullptr), next(nullptr) {}
    };

    atomic<Node*> head;
    atomic<Node*> tail;

public:
    LockFreeQueue() {
        Node* dummy = new Node();
        head.store(dummy);
        tail.store(dummy);
    }

    void enqueue(const T& item) {
        Node* newNode = new Node();
        T* dataPtr = new T(item);
        newNode->data.store(dataPtr);

        Node* prevTail = tail.exchange(newNode);
        prevTail->next.store(newNode);
    }

    bool dequeue(T& result) {
        Node* headNode = head.load();
        Node* next = headNode->next.load();

        if (next == nullptr) {
            return false;
        }

        T* dataPtr = next->data.load();
        if (dataPtr == nullptr) {
            return false;
        }

        result = *dataPtr;
        head.store(next);
        delete dataPtr;
        delete headNode;
        return true;
    }

    bool isEmpty() const {
        return head.load()->next.load() == nullptr;
    }
};

int main() {
    LockFreeStack<int> stack;

    // Test single thread
    stack.push(1);
    stack.push(2);
    stack.push(3);

    int val;
    while (stack.pop(val)) {
        cout << "Popped: " << val << endl;
    }

    // Test multi-threaded
    vector<thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.emplace_back([&stack, i]() {
            stack.push(i);
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    cout << "Multi-threaded push completed" << endl;

    LockFreeQueue<int> queue;
    queue.enqueue(10);
    queue.enqueue(20);

    int result;
    while (queue.dequeue(result)) {
        cout << "Dequeued: " << result << endl;
    }

    return 0;
}

