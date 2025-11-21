#include <iostream>
#include <vector>
#include <bitset>
#include <functional>
#include <string>

using namespace std;

// Bloom Filter - Probabilistic data structure
// Space efficient membership testing with false positives possible
// Time: O(k) insert/query, Space: O(m) where m is bit array size
class BloomFilter {
private:
    vector<bool> bits;
    size_t size;
    size_t numHashFunctions;
    
    // Multiple hash functions using different seeds
    // Using FNV-1a hash with different seeds for variety
    size_t hash1(const string& key) const {
        size_t hash = 2166136261u;
        for (char c : key) {
            hash ^= c;
            hash *= 16777619u;
        }
        return hash % size;
    }

    size_t hash2(const string& key) const {
        size_t hash = 5381;
        for (char c : key) {
            hash = ((hash << 5) + hash) + c;
        }
        return hash % size;
    }

    size_t hash3(const string& key) const {
        size_t hash = 0;
        for (char c : key) {
            hash = c + (hash << 6) + (hash << 16) - hash;
        }
        return hash % size;
    }

    size_t hash4(const string& key) const {
        size_t hash = 1315423911;
        for (char c : key) {
            hash ^= ((hash << 5) + c + (hash >> 2));
        }
        return hash % size;
    }

    // Get all hash values
    vector<size_t> getHashes(const string& key) const {
        vector<size_t> hashes;
        hashes.push_back(hash1(key));
        hashes.push_back(hash2(key));
        hashes.push_back(hash3(key));
        hashes.push_back(hash4(key));
        
        // Generate additional hashes by combining
        for (size_t i = 4; i < numHashFunctions; i++) {
            hashes.push_back((hashes[i-4] + i * hashes[i-3]) % size);
        }
        return hashes;
    }

public:
    // Optimal size calculation: m = -n*ln(p) / (ln(2)^2)
    // Optimal hash functions: k = (m/n) * ln(2)
    BloomFilter(size_t expectedElements, double falsePositiveRate = 0.01) {
        size = static_cast<size_t>(-expectedElements * log(falsePositiveRate) / (log(2) * log(2)));
        numHashFunctions = static_cast<size_t>((size / expectedElements) * log(2));
        if (numHashFunctions < 1) numHashFunctions = 1;
        if (numHashFunctions > 10) numHashFunctions = 10; // Cap at 10
        
        bits.resize(size, false);
        cout << "Bloom Filter initialized: size=" << size 
             << ", hash functions=" << numHashFunctions << endl;
    }

    void insert(const string& key) {
        vector<size_t> hashes = getHashes(key);
        for (size_t hash : hashes) {
            bits[hash] = true;
        }
    }

    bool contains(const string& key) const {
        vector<size_t> hashes = getHashes(key);
        for (size_t hash : hashes) {
            if (!bits[hash]) {
                return false; // Definitely not present
            }
        }
        return true; // Probably present (may be false positive)
    }

    void clear() {
        fill(bits.begin(), bits.end(), false);
    }

    size_t getSize() const { return size; }
    size_t getHashFunctions() const { return numHashFunctions; }
};

// Counting Bloom Filter - Supports deletion
class CountingBloomFilter {
private:
    vector<int> counters;
    size_t size;
    size_t numHashFunctions;

    size_t hash1(const string& key) const {
        size_t hash = 2166136261u;
        for (char c : key) {
            hash ^= c;
            hash *= 16777619u;
        }
        return hash % size;
    }

    size_t hash2(const string& key) const {
        size_t hash = 5381;
        for (char c : key) {
            hash = ((hash << 5) + hash) + c;
        }
        return hash % size;
    }

    vector<size_t> getHashes(const string& key) const {
        vector<size_t> hashes = {hash1(key), hash2(key)};
        for (size_t i = 2; i < numHashFunctions; i++) {
            hashes.push_back((hashes[i-2] + i * hashes[i-1]) % size);
        }
        return hashes;
    }

public:
    CountingBloomFilter(size_t expectedElements, double falsePositiveRate = 0.01) {
        size = static_cast<size_t>(-expectedElements * log(falsePositiveRate) / (log(2) * log(2)));
        numHashFunctions = static_cast<size_t>((size / expectedElements) * log(2));
        if (numHashFunctions < 1) numHashFunctions = 1;
        counters.resize(size, 0);
    }

    void insert(const string& key) {
        vector<size_t> hashes = getHashes(key);
        for (size_t hash : hashes) {
            counters[hash]++;
        }
    }

    void remove(const string& key) {
        vector<size_t> hashes = getHashes(key);
        for (size_t hash : hashes) {
            if (counters[hash] > 0) {
                counters[hash]--;
            }
        }
    }

    bool contains(const string& key) const {
        vector<size_t> hashes = getHashes(key);
        for (size_t hash : hashes) {
            if (counters[hash] == 0) {
                return false;
            }
        }
        return true;
    }
};

int main() {
    BloomFilter bf(1000, 0.01);

    bf.insert("apple");
    bf.insert("banana");
    bf.insert("cherry");

    cout << "Contains 'apple': " << bf.contains("apple") << endl;
    cout << "Contains 'banana': " << bf.contains("banana") << endl;
    cout << "Contains 'grape': " << bf.contains("grape") << endl; // False positive possible

    CountingBloomFilter cbf(1000, 0.01);
    cbf.insert("test");
    cout << "CBF contains 'test': " << cbf.contains("test") << endl;
    cbf.remove("test");
    cout << "CBF contains 'test' after remove: " << cbf.contains("test") << endl;

    return 0;
}

