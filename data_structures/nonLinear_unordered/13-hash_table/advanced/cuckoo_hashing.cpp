#include <iostream>
#include <vector>
#include <functional>
#include <cstdlib>

using namespace std;

// Cuckoo Hashing - Uses two hash tables with two hash functions
// O(1) expected insert/lookup/delete
// May need rehashing if cycles occur
template<typename K, typename V>
class CuckooHashTable {
private:
    struct Entry {
        K key;
        V value;
        bool occupied;

        Entry() : occupied(false) {}
    };

    vector<Entry> table1;
    vector<Entry> table2;
    size_t size;
    size_t capacity;
    size_t maxIterations;

    hash<K> hash1;
    hash<K> hash2;

    size_t getHash1(const K& key) const {
        return hash1(key) % capacity;
    }

    size_t getHash2(const K& key) const {
        return hash2(key) % capacity;
    }

    void rehash() {
        vector<Entry> oldTable1 = table1;
        vector<Entry> oldTable2 = table2;
        size_t oldCapacity = capacity;

        capacity *= 2;
        table1.clear();
        table2.clear();
        table1.resize(capacity);
        table2.resize(capacity);
        size = 0;

        for (size_t i = 0; i < oldCapacity; i++) {
            if (oldTable1[i].occupied) {
                insert(oldTable1[i].key, oldTable1[i].value);
            }
            if (oldTable2[i].occupied) {
                insert(oldTable2[i].key, oldTable2[i].value);
            }
        }
    }

public:
    CuckooHashTable(size_t cap = 16) : capacity(cap), size(0), maxIterations(100) {
        table1.resize(capacity);
        table2.resize(capacity);
    }

    bool insert(const K& key, const V& value) {
        if (contains(key)) {
            return false;
        }

        K currentKey = key;
        V currentValue = value;
        bool useTable1 = true;
        size_t iterations = 0;

        while (iterations < maxIterations) {
            if (useTable1) {
                size_t pos = getHash1(currentKey);
                if (!table1[pos].occupied) {
                    table1[pos].key = currentKey;
                    table1[pos].value = currentValue;
                    table1[pos].occupied = true;
                    size++;
                    return true;
                }
                swap(currentKey, table1[pos].key);
                swap(currentValue, table1[pos].value);
            } else {
                size_t pos = getHash2(currentKey);
                if (!table2[pos].occupied) {
                    table2[pos].key = currentKey;
                    table2[pos].value = currentValue;
                    table2[pos].occupied = true;
                    size++;
                    return true;
                }
                swap(currentKey, table2[pos].key);
                swap(currentValue, table2[pos].value);
            }
            useTable1 = !useTable1;
            iterations++;
        }

        // Rehash if we hit max iterations
        rehash();
        return insert(currentKey, currentValue);
    }

    bool contains(const K& key) const {
        size_t pos1 = getHash1(key);
        size_t pos2 = getHash2(key);

        return (table1[pos1].occupied && table1[pos1].key == key) ||
               (table2[pos2].occupied && table2[pos2].key == key);
    }

    V* get(const K& key) {
        size_t pos1 = getHash1(key);
        if (table1[pos1].occupied && table1[pos1].key == key) {
            return &table1[pos1].value;
        }

        size_t pos2 = getHash2(key);
        if (table2[pos2].occupied && table2[pos2].key == key) {
            return &table2[pos2].value;
        }

        return nullptr;
    }

    bool remove(const K& key) {
        size_t pos1 = getHash1(key);
        if (table1[pos1].occupied && table1[pos1].key == key) {
            table1[pos1].occupied = false;
            size--;
            return true;
        }

        size_t pos2 = getHash2(key);
        if (table2[pos2].occupied && table2[pos2].key == key) {
            table2[pos2].occupied = false;
            size--;
            return true;
        }

        return false;
    }

    size_t getSize() const { return size; }
    size_t getCapacity() const { return capacity; }
};

int main() {
    CuckooHashTable<int, string> table;

    table.insert(1, "one");
    table.insert(2, "two");
    table.insert(3, "three");
    table.insert(4, "four");

    cout << "Contains 2: " << table.contains(2) << endl;
    cout << "Contains 5: " << table.contains(5) << endl;

    string* value = table.get(3);
    if (value) {
        cout << "Value at 3: " << *value << endl;
    }

    table.remove(2);
    cout << "After remove, contains 2: " << table.contains(2) << endl;

    return 0;
}

