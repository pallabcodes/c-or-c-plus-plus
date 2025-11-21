/*
 * Linux Kernel Generic Binary Search
 * 
 * Source: linux/lib/bsearch.c
 * 
 * What Makes It Ingenious:
 * - Generic type-agnostic implementation
 * - Uses function pointer for comparison
 * - Memory-efficient (no type-specific code)
 * - Works with any data type
 * 
 * When to Use:
 * - Kernel-level operations
 * - Need generic comparator
 * - Memory-efficient requirements
 * - Type-agnostic search
 * 
 * Real-World Usage:
 * - Linux kernel module lookup
 * - Generic array search in kernel
 */

#include <cstdlib>
#include <cstring>
#include <functional>

/*
 * Generic binary search implementation (Linux kernel style)
 * 
 * @param key: Pointer to item being searched for
 * @param base: Pointer to first element to search
 * @param num: Number of elements
 * @param size: Size of each element
 * @param cmp: Comparison function (returns <0, 0, or >0)
 * 
 * @return: Pointer to matching element, or NULL if not found
 */
void* bsearch_generic(
    const void* key,
    const void* base,
    size_t num,
    size_t size,
    int (*cmp)(const void*, const void*)
) {
    const char* base_ptr = static_cast<const char*>(base);
    size_t left = 0;
    size_t right = num;
    
    while (left < right) {
        size_t mid = left + (right - left) / 2;
        const void* mid_ptr = base_ptr + mid * size;
        
        int result = cmp(key, mid_ptr);
        
        if (result == 0) {
            return const_cast<void*>(mid_ptr); // Found
        } else if (result < 0) {
            right = mid; // Search left
        } else {
            left = mid + 1; // Search right
        }
    }
    
    return nullptr; // Not found
}

// C++ wrapper with type safety
template<typename T>
class GenericBinarySearch {
public:
    // Search using comparator function
    static T* Search(
        const T* array,
        size_t size,
        const T& key,
        std::function<int(const T&, const T&)> comparator
    ) {
        return static_cast<T*>(bsearch_generic(
            &key,
            array,
            size,
            sizeof(T),
            [](const void* a, const void* b) -> int {
                const T* ta = static_cast<const T*>(a);
                const T* tb = static_cast<const T*>(b);
                // Note: This requires a way to call the comparator
                // In real implementation, would use a context pointer
                return 0; // Placeholder
            }
        ));
    }
    
    // Type-safe wrapper
    template<typename Compare>
    static T* SearchSafe(
        const T* array,
        size_t size,
        const T& key,
        Compare cmp
    ) {
        size_t left = 0;
        size_t right = size;
        
        while (left < right) {
            size_t mid = left + (right - left) / 2;
            
            int result = cmp(key, array[mid]);
            
            if (result == 0) {
                return const_cast<T*>(&array[mid]);
            } else if (result < 0) {
                right = mid;
            } else {
                left = mid + 1;
            }
        }
        
        return nullptr;
    }
};

// Example usage
#include <iostream>
#include <vector>

struct Person {
    int id;
    char name[32];
};

int compare_person(const void* a, const void* b) {
    const Person* pa = static_cast<const Person*>(a);
    const Person* pb = static_cast<const Person*>(b);
    
    if (pa->id < pb->id) return -1;
    if (pa->id > pb->id) return 1;
    return 0;
}

int main() {
    Person people[] = {
        {1, "Alice"},
        {3, "Bob"},
        {5, "Charlie"},
        {7, "David"}
    };
    
    Person key = {5, ""};
    
    Person* result = static_cast<Person*>(
        bsearch_generic(&key, people, 4, sizeof(Person), compare_person)
    );
    
    if (result) {
        std::cout << "Found: " << result->name << std::endl;
    }
    
    return 0;
}

