#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstring>

using namespace std;

// Memory Pool Allocator - Pre-allocates memory chunks
// Reduces allocation overhead for frequent allocations
template<typename T>
class MemoryPool {
private:
    struct Chunk {
        char data[sizeof(T)];
        Chunk* next;
    };

    Chunk* freeList;
    vector<void*> blocks;
    size_t chunkSize;
    size_t chunksPerBlock;
    size_t totalChunks;

    void allocateBlock() {
        Chunk* block = static_cast<Chunk*>(malloc(chunksPerBlock * sizeof(Chunk)));
        if (!block) {
            throw bad_alloc();
        }

        blocks.push_back(block);

        // Link chunks in free list
        for (size_t i = 0; i < chunksPerBlock - 1; i++) {
            block[i].next = &block[i + 1];
        }
        block[chunksPerBlock - 1].next = freeList;
        freeList = block;
        totalChunks += chunksPerBlock;
    }

public:
    MemoryPool(size_t chunksPerBlock = 1024) 
        : freeList(nullptr), chunkSize(sizeof(T)), 
          chunksPerBlock(chunksPerBlock), totalChunks(0) {
        allocateBlock();
    }

    ~MemoryPool() {
        for (void* block : blocks) {
            free(block);
        }
    }

    T* allocate() {
        if (!freeList) {
            allocateBlock();
        }

        Chunk* chunk = freeList;
        freeList = freeList->next;
        return reinterpret_cast<T*>(chunk);
    }

    void deallocate(T* ptr) {
        if (!ptr) {
            return;
        }

        Chunk* chunk = reinterpret_cast<Chunk*>(ptr);
        chunk->next = freeList;
        freeList = chunk;
    }

    size_t getTotalChunks() const {
        return totalChunks;
    }
};

// Pool-allocated vector for demonstration
template<typename T>
class PoolVector {
private:
    MemoryPool<T> pool;
    T* data;
    size_t size;
    size_t capacity;

    void resize(size_t newCapacity) {
        T* newData = pool.allocate();
        for (size_t i = 0; i < size; i++) {
            new (newData + i) T(data[i]);
            (data + i)->~T();
        }
        if (data) {
            pool.deallocate(data);
        }
        data = newData;
        capacity = newCapacity;
    }

public:
    PoolVector() : size(0), capacity(1) {
        data = pool.allocate();
    }

    ~PoolVector() {
        for (size_t i = 0; i < size; i++) {
            (data + i)->~T();
        }
        if (data) {
            pool.deallocate(data);
        }
    }

    void push_back(const T& item) {
        if (size >= capacity) {
            resize(capacity * 2);
        }
        new (data + size) T(item);
        size++;
    }

    T& operator[](size_t index) {
        return data[index];
    }

    size_t getSize() const {
        return size;
    }
};

int main() {
    MemoryPool<int> pool(100);

    int* ptr1 = pool.allocate();
    *ptr1 = 42;
    cout << "Allocated value: " << *ptr1 << endl;

    int* ptr2 = pool.allocate();
    *ptr2 = 100;
    cout << "Allocated value: " << *ptr2 << endl;

    pool.deallocate(ptr1);
    pool.deallocate(ptr2);

    cout << "Total chunks: " << pool.getTotalChunks() << endl;

    // Pool-allocated vector example
    PoolVector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    cout << "PoolVector size: " << vec.getSize() << endl;
    cout << "PoolVector[0]: " << vec[0] << endl;

    return 0;
}

