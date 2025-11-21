#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <bitset>
#include <climits>

using namespace std;

// Roaring Bitmap - Hybrid compressed bitmap structure
// Combines arrays for sparse data and bitmaps for dense data
// Memory efficient and fast for set operations
class RoaringBitmap {
private:
    struct Container {
        vector<uint16_t> array; // For sparse containers (< 4096 elements)
        bitset<65536> bitmap;   // For dense containers (>= 4096 elements)
        bool isArray;

        Container() : isArray(true) {}

        void add(uint16_t value) {
            if (isArray) {
                if (array.size() >= 4096) {
                    // Convert to bitmap
                    for (uint16_t v : array) {
                        bitmap.set(v);
                    }
                    array.clear();
                    isArray = false;
                } else {
                    // Insert in sorted order
                    auto it = lower_bound(array.begin(), array.end(), value);
                    if (it == array.end() || *it != value) {
                        array.insert(it, value);
                    }
                }
            } else {
                bitmap.set(value);
            }
        }

        bool contains(uint16_t value) const {
            if (isArray) {
                return binary_search(array.begin(), array.end(), value);
            } else {
                return bitmap.test(value);
            }
        }

        size_t cardinality() const {
            if (isArray) {
                return array.size();
            } else {
                return bitmap.count();
            }
        }
    };

    unordered_map<uint16_t, Container> containers; // High 16 bits -> Container

    uint16_t highBits(uint32_t value) const {
        return static_cast<uint16_t>(value >> 16);
    }

    uint16_t lowBits(uint32_t value) const {
        return static_cast<uint16_t>(value & 0xFFFF);
    }

public:
    void add(uint32_t value) {
        uint16_t high = highBits(value);
        uint16_t low = lowBits(value);
        containers[high].add(low);
    }

    bool contains(uint32_t value) const {
        uint16_t high = highBits(value);
        uint16_t low = lowBits(value);
        auto it = containers.find(high);
        if (it == containers.end()) {
            return false;
        }
        return it->second.contains(low);
    }

    size_t cardinality() const {
        size_t count = 0;
        for (const auto& pair : containers) {
            count += pair.second.cardinality();
        }
        return count;
    }

    // Union operation
    RoaringBitmap unionWith(const RoaringBitmap& other) const {
        RoaringBitmap result;
        result.containers = containers;

        for (const auto& pair : other.containers) {
            uint16_t high = pair.first;
            const Container& otherContainer = pair.second;

            if (result.containers.find(high) == result.containers.end()) {
                result.containers[high] = otherContainer;
            } else {
                Container& resultContainer = result.containers[high];
                // Simple union: add all elements from other container
                if (otherContainer.isArray) {
                    for (uint16_t v : otherContainer.array) {
                        resultContainer.add(v);
                    }
                } else {
                    // Convert result to bitmap if needed, then OR
                    if (resultContainer.isArray) {
                        for (uint16_t v : resultContainer.array) {
                            resultContainer.bitmap.set(v);
                        }
                        resultContainer.array.clear();
                        resultContainer.isArray = false;
                    }
                    resultContainer.bitmap |= otherContainer.bitmap;
                }
            }
        }
        return result;
    }

    // Intersection operation
    RoaringBitmap intersectWith(const RoaringBitmap& other) const {
        RoaringBitmap result;

        for (const auto& pair : containers) {
            uint16_t high = pair.first;
            auto otherIt = other.containers.find(high);
            if (otherIt == other.containers.end()) {
                continue;
            }

            const Container& thisContainer = pair.second;
            const Container& otherContainer = otherIt->second;

            Container resultContainer;

            if (thisContainer.isArray && otherContainer.isArray) {
                // Array intersection
                for (uint16_t v : thisContainer.array) {
                    if (otherContainer.contains(v)) {
                        resultContainer.add(v);
                    }
                }
            } else {
                // At least one is bitmap - convert both and intersect
                bitset<65536> thisBits, otherBits;
                if (thisContainer.isArray) {
                    for (uint16_t v : thisContainer.array) {
                        thisBits.set(v);
                    }
                } else {
                    thisBits = thisContainer.bitmap;
                }

                if (otherContainer.isArray) {
                    for (uint16_t v : otherContainer.array) {
                        otherBits.set(v);
                    }
                } else {
                    otherBits = otherContainer.bitmap;
                }

                thisBits &= otherBits;
                resultContainer.bitmap = thisBits;
                resultContainer.isArray = false;
            }

            if (resultContainer.cardinality() > 0) {
                result.containers[high] = resultContainer;
            }
        }
        return result;
    }

    void clear() {
        containers.clear();
    }
};

int main() {
    RoaringBitmap rb1, rb2;

    // Add elements
    for (int i = 0; i < 1000; i += 2) {
        rb1.add(i);
    }

    for (int i = 500; i < 1500; i += 3) {
        rb2.add(i);
    }

    cout << "RB1 cardinality: " << rb1.cardinality() << endl;
    cout << "RB2 cardinality: " << rb2.cardinality() << endl;
    cout << "RB1 contains 100: " << rb1.contains(100) << endl;
    cout << "RB1 contains 101: " << rb1.contains(101) << endl;

    RoaringBitmap rbUnion = rb1.unionWith(rb2);
    cout << "Union cardinality: " << rbUnion.cardinality() << endl;

    RoaringBitmap rbIntersect = rb1.intersectWith(rb2);
    cout << "Intersection cardinality: " << rbIntersect.cardinality() << endl;

    return 0;
}

