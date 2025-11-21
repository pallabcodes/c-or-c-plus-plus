#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>

using namespace std;

// Robin Hood Hashing - Open addressing with distance from ideal position
// Reduces variance in probe lengths
// O(1) amortized operations
template<typename K, typename V>
class RobinHoodHashTable {
private:
    struct Entry {
        K key;
        V value;
        size_t distance;
        bool occupied;

        Entry() : distance(0), occupied(false) {}
    };

    vector<Entry> table;
    size_t size;
    size_t capacity;
    double loadFactorThreshold;

    hash<K> hasher;

    size_t getHash(const K& key) const {
        return hasher(key) % capacity;
    }

    void rehash() {
        vector<Entry> oldTable = table;
        size_t oldCapacity = capacity;

        capacity *= 2;
        table.clear();
        table.resize(capacity);
        size = 0;

        for (size_t i = 0; i < oldCapacity; i++) {
            if (oldTable[i].occupied) {
                insert(oldTable[i].key, oldTable[i].value);
            }
        }
    }

public:
    RobinHoodHashTable(size_t cap = 16, double threshold = 0.75) 
        : capacity(cap), size(0), loadFactorThreshold(threshold) {
        table.resize(capacity);
    }

    bool insert(const K& key, const V& value) {
        if (static_cast<double>(size) / capacity >= loadFactorThreshold) {
            rehash();
        }

        size_t pos = getHash(key);
        size_t distance = 0;
        Entry entry;
        entry.key = key;
        entry.value = value;
        entry.distance = distance;
        entry.occupied = true;

        while (true) {
            if (!table[pos].occupied) {
                table[pos] = entry;
                size++;
                return true;
            }

            if (table[pos].key == key) {
                table[pos].value = value;
                return false;
            }

            if (distance > table[pos].distance) {
                swap(entry, table[pos]);
            }

            pos = (pos + 1) % capacity;
            distance++;
        }
    }

    bool contains(const K& key) const {
        size_t pos = getHash(key);
        size_t distance = 0;

        while (table[pos].occupied && distance <= table[pos].distance) {
            if (table[pos].key == key) {
                return true;
            }
            pos = (pos + 1) % capacity;
            distance++;
        }

        return false;
    }

    V* get(const K& key) {
        size_t pos = getHash(key);
        size_t distance = 0;

        while (table[pos].occupied && distance <= table[pos].distance) {
            if (table[pos].key == key) {
                return &table[pos].value;
            }
            pos = (pos + 1) % capacity;
            distance++;
        }

        return nullptr;
    }

    bool remove(const K& key) {
        size_t pos = getHash(key);
        size_t distance = 0;

        while (table[pos].occupied && distance <= table[pos].distance) {
            if (table[pos].key == key) {
                table[pos].occupied = false;
                size--;

                // Backward shift
                size_t nextPos = (pos + 1) % capacity;
                while (table[nextPos].occupied && table[nextPos].distance > 0) {
                    table[pos] = table[nextPos];
                    table[pos].distance--;
                    table[nextPos].occupied = false;
                    pos = nextPos;
                    nextPos = (nextPos + 1) % capacity;
                }

                return true;
            }
            pos = (pos + 1) % capacity;
            distance++;
        }

        return false;
    }

    size_t getSize() const { return size; }
    size_t getCapacity() const { return capacity; }
};

int main() {
    RobinHoodHashTable<int, string> table;

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

