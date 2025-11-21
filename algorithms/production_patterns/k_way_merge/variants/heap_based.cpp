/*
 * K-way Merge - Heap-Based Pattern
 * 
 * Source: Generic pattern, commonly used in production
 * 
 * What Makes It Ingenious:
 * - O(N log K) time complexity (N = total elements, K = sequences)
 * - O(K) space complexity (only K elements in heap)
 * - Works with any number of sequences
 * - Can stop early if needed (e.g., find Kth smallest)
 * 
 * When to Use:
 * - K sorted sequences (K is small to medium, < 100)
 * - Need full merged result
 * - Random access to sequences
 * - Can use priority queue
 * 
 * Real-World Usage:
 * - Merge K sorted lists
 * - External sorting merge phase
 * - Database merge joins
 * - Log file merging
 * - Search engine result merging
 */

#include <vector>
#include <queue>
#include <iostream>

// Example 1: Merge K Sorted Lists (using ListNode)
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

struct Compare {
    bool operator()(const ListNode* a, const ListNode* b) {
        return a->val > b->val; // Min-heap
    }
};

ListNode* mergeKLists(std::vector<ListNode*>& lists) {
    // Min-heap to store current smallest element from each list
    std::priority_queue<ListNode*, std::vector<ListNode*>, Compare> pq;
    
    // Initialize heap with first element from each list
    for (ListNode* list : lists) {
        if (list) {
            pq.push(list);
        }
    }
    
    // Dummy head for result list
    ListNode* dummy = new ListNode(0);
    ListNode* curr = dummy;
    
    while (!pq.empty()) {
        // Get smallest element
        ListNode* node = pq.top();
        pq.pop();
        
        // Add to result
        curr->next = node;
        curr = curr->next;
        
        // Add next element from same list
        if (node->next) {
            pq.push(node->next);
        }
    }
    
    ListNode* result = dummy->next;
    delete dummy;
    return result;
}

// Example 2: Merge K Sorted Arrays
std::vector<int> mergeKSortedArrays(const std::vector<std::vector<int>>& arrays) {
    // Min-heap stores {value, array_index, element_index}
    using Element = std::tuple<int, int, int>;
    std::priority_queue<Element, std::vector<Element>, std::greater<Element>> pq;
    
    // Initialize heap with first element from each array
    for (size_t i = 0; i < arrays.size(); i++) {
        if (!arrays[i].empty()) {
            pq.push({arrays[i][0], static_cast<int>(i), 0});
        }
    }
    
    std::vector<int> result;
    
    while (!pq.empty()) {
        auto [val, arr_idx, elem_idx] = pq.top();
        pq.pop();
        
        result.push_back(val);
        
        // Add next element from same array
        if (elem_idx + 1 < arrays[arr_idx].size()) {
            pq.push({arrays[arr_idx][elem_idx + 1], arr_idx, elem_idx + 1});
        }
    }
    
    return result;
}

// Example 3: Find Kth Smallest in K Sorted Arrays
int findKthSmallest(const std::vector<std::vector<int>>& arrays, int k) {
    using Element = std::tuple<int, int, int>;
    std::priority_queue<Element, std::vector<Element>, std::greater<Element>> pq;
    
    // Initialize heap
    for (size_t i = 0; i < arrays.size(); i++) {
        if (!arrays[i].empty()) {
            pq.push({arrays[i][0], static_cast<int>(i), 0});
        }
    }
    
    int count = 0;
    int result = 0;
    
    while (!pq.empty() && count < k) {
        auto [val, arr_idx, elem_idx] = pq.top();
        pq.pop();
        
        count++;
        if (count == k) {
            result = val;
            break;
        }
        
        // Add next element from same array
        if (elem_idx + 1 < arrays[arr_idx].size()) {
            pq.push({arrays[arr_idx][elem_idx + 1], arr_idx, elem_idx + 1});
        }
    }
    
    return result;
}

// Example 4: Merge K Sorted Arrays (In-Place Style with Iterators)
template<typename Iterator>
std::vector<typename std::iterator_traits<Iterator>::value_type> 
mergeKSortedRanges(const std::vector<std::pair<Iterator, Iterator>>& ranges) {
    using ValueType = typename std::iterator_traits<Iterator>::value_type;
    using Element = std::tuple<ValueType, size_t>;
    
    std::priority_queue<Element, std::vector<Element>, std::greater<Element>> pq;
    std::vector<Iterator> current_iters;
    
    // Initialize heap and iterators
    for (const auto& [begin, end] : ranges) {
        if (begin != end) {
            pq.push({*begin, current_iters.size()});
            current_iters.push_back(begin);
        }
    }
    
    std::vector<ValueType> result;
    
    while (!pq.empty()) {
        auto [val, range_idx] = pq.top();
        pq.pop();
        
        result.push_back(val);
        
        // Advance iterator
        current_iters[range_idx]++;
        
        // Check if range has more elements
        const auto& [begin, end] = ranges[range_idx];
        if (current_iters[range_idx] != end) {
            pq.push({*current_iters[range_idx], range_idx});
        }
    }
    
    return result;
}

// Example 5: External Sort Merge Phase (Streaming)
class ExternalSortMerger {
private:
    struct StreamElement {
        int value;
        int stream_id;
        
        bool operator>(const StreamElement& other) const {
            return value > other.value; // Min-heap
        }
    };
    
    std::priority_queue<StreamElement, std::vector<StreamElement>, 
                       std::greater<StreamElement>> pq;
    
public:
    // Add first element from a stream
    void addStream(int stream_id, int first_value) {
        pq.push({first_value, stream_id});
    }
    
    // Get next smallest element (returns stream_id for next read)
    std::pair<int, int> getNext() {
        if (pq.empty()) {
            return {-1, -1}; // No more elements
        }
        
        StreamElement elem = pq.top();
        pq.pop();
        
        return {elem.value, elem.stream_id};
    }
    
    // Add next element from a stream
    void addNextFromStream(int stream_id, int value) {
        pq.push({value, stream_id});
    }
    
    bool hasMore() const {
        return !pq.empty();
    }
};

// Example usage
int main() {
    // Example 1: Merge K sorted arrays
    std::vector<std::vector<int>> arrays = {
        {1, 4, 7},
        {2, 5, 8},
        {3, 6, 9}
    };
    
    std::vector<int> merged = mergeKSortedArrays(arrays);
    std::cout << "Merged array: ";
    for (int val : merged) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    
    // Example 2: Find Kth smallest
    int kth = findKthSmallest(arrays, 5);
    std::cout << "5th smallest: " << kth << std::endl;
    
    // Example 3: External sort merger
    ExternalSortMerger merger;
    merger.addStream(0, 1);
    merger.addStream(1, 2);
    merger.addStream(2, 3);
    
    std::cout << "External sort merge: ";
    while (merger.hasMore()) {
        auto [value, stream_id] = merger.getNext();
        std::cout << value << " ";
        // In real scenario, would read next from stream_id
    }
    std::cout << std::endl;
    
    return 0;
}

