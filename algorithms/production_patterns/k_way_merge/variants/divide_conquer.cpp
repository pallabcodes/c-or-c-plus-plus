/*
 * K-way Merge - Divide-and-Conquer Pattern
 * 
 * Source: Generic pattern, commonly used in production
 * 
 * What Makes It Ingenious:
 * - O(N log K) time complexity (same as heap, but better constant factors)
 * - Better cache locality (merging adjacent pairs)
 * - Reduces heap overhead for large K
 * - Recursive structure is elegant
 * 
 * When to Use:
 * - K is large (> 100)
 * - Want better cache performance
 * - Want to reduce heap overhead
 * - Sequences are similar in size
 * 
 * Real-World Usage:
 * - External sorting (large K)
 * - Database merge joins (many tables)
 * - Large-scale log merging
 * - Distributed system result merging
 */

#include <vector>
#include <iostream>

// Example 1: Merge Two Sorted Arrays (Helper)
std::vector<int> mergeTwoArrays(const std::vector<int>& arr1, 
                                 const std::vector<int>& arr2) {
    std::vector<int> result;
    result.reserve(arr1.size() + arr2.size());
    
    int i = 0, j = 0;
    
    while (i < arr1.size() && j < arr2.size()) {
        if (arr1[i] <= arr2[j]) {
            result.push_back(arr1[i++]);
        } else {
            result.push_back(arr2[j++]);
        }
    }
    
    // Copy remaining elements
    while (i < arr1.size()) {
        result.push_back(arr1[i++]);
    }
    while (j < arr2.size()) {
        result.push_back(arr2[j++]);
    }
    
    return result;
}

// Example 2: Merge K Sorted Arrays (Divide-and-Conquer)
std::vector<int> mergeKSortedArraysDC(const std::vector<std::vector<int>>& arrays, 
                                       int left, int right) {
    // Base case: single array
    if (left == right) {
        return arrays[left];
    }
    
    // Base case: two arrays
    if (left + 1 == right) {
        return mergeTwoArrays(arrays[left], arrays[right]);
    }
    
    // Divide: split in half
    int mid = (left + right) / 2;
    
    // Conquer: merge left half and right half recursively
    std::vector<int> leftMerged = mergeKSortedArraysDC(arrays, left, mid);
    std::vector<int> rightMerged = mergeKSortedArraysDC(arrays, mid + 1, right);
    
    // Combine: merge the two merged halves
    return mergeTwoArrays(leftMerged, rightMerged);
}

// Wrapper function
std::vector<int> mergeKSortedArraysDC(const std::vector<std::vector<int>>& arrays) {
    if (arrays.empty()) {
        return {};
    }
    if (arrays.size() == 1) {
        return arrays[0];
    }
    return mergeKSortedArraysDC(arrays, 0, arrays.size() - 1);
}

// Example 3: Merge K Sorted Arrays (Iterative Divide-and-Conquer)
std::vector<int> mergeKSortedArraysIterative(const std::vector<std::vector<int>>& arrays) {
    if (arrays.empty()) {
        return {};
    }
    
    // Start with individual arrays
    std::vector<std::vector<int>> current = arrays;
    
    // Keep merging pairs until one array remains
    while (current.size() > 1) {
        std::vector<std::vector<int>> next;
        
        // Merge pairs
        for (size_t i = 0; i < current.size(); i += 2) {
            if (i + 1 < current.size()) {
                // Merge two arrays
                next.push_back(mergeTwoArrays(current[i], current[i + 1]));
            } else {
                // Odd one out, keep as is
                next.push_back(current[i]);
            }
        }
        
        current = next;
    }
    
    return current[0];
}

// Example 4: Merge K Sorted Lists (Divide-and-Conquer)
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
    ListNode* dummy = new ListNode(0);
    ListNode* curr = dummy;
    
    while (l1 && l2) {
        if (l1->val <= l2->val) {
            curr->next = l1;
            l1 = l1->next;
        } else {
            curr->next = l2;
            l2 = l2->next;
        }
        curr = curr->next;
    }
    
    curr->next = l1 ? l1 : l2;
    
    ListNode* result = dummy->next;
    delete dummy;
    return result;
}

ListNode* mergeKListsDC(std::vector<ListNode*>& lists, int left, int right) {
    if (left == right) {
        return lists[left];
    }
    
    if (left + 1 == right) {
        return mergeTwoLists(lists[left], lists[right]);
    }
    
    int mid = (left + right) / 2;
    ListNode* leftMerged = mergeKListsDC(lists, left, mid);
    ListNode* rightMerged = mergeKListsDC(lists, mid + 1, right);
    
    return mergeTwoLists(leftMerged, rightMerged);
}

ListNode* mergeKListsDC(std::vector<ListNode*>& lists) {
    if (lists.empty()) {
        return nullptr;
    }
    if (lists.size() == 1) {
        return lists[0];
    }
    return mergeKListsDC(lists, 0, lists.size() - 1);
}

// Example 5: External Sort Merge (Divide-and-Conquer Style)
class ExternalSortMergerDC {
private:
    std::vector<std::vector<int>> runs;
    
    std::vector<int> mergeTwoRuns(const std::vector<int>& run1, 
                                   const std::vector<int>& run2) {
        return mergeTwoArrays(run1, run2);
    }
    
public:
    void addRun(const std::vector<int>& run) {
        runs.push_back(run);
    }
    
    std::vector<int> mergeAll() {
        if (runs.empty()) {
            return {};
        }
        return mergeKSortedArraysDC(runs);
    }
    
    // Merge in batches (useful for external sort)
    void mergeInBatches(int batchSize) {
        while (runs.size() > 1) {
            std::vector<std::vector<int>> nextBatch;
            
            for (size_t i = 0; i < runs.size(); i += batchSize) {
                std::vector<std::vector<int>> batch;
                for (size_t j = i; j < std::min(i + batchSize, runs.size()); j++) {
                    batch.push_back(runs[j]);
                }
                
                if (batch.size() == 1) {
                    nextBatch.push_back(batch[0]);
                } else {
                    nextBatch.push_back(mergeKSortedArraysDC(batch));
                }
            }
            
            runs = nextBatch;
        }
    }
    
    std::vector<int> getResult() {
        return runs.empty() ? std::vector<int>() : runs[0];
    }
};

// Example usage
int main() {
    // Example 1: Merge K sorted arrays (recursive)
    std::vector<std::vector<int>> arrays = {
        {1, 4, 7, 10},
        {2, 5, 8, 11},
        {3, 6, 9, 12},
        {13, 14, 15, 16}
    };
    
    std::vector<int> merged = mergeKSortedArraysDC(arrays);
    std::cout << "Merged (recursive): ";
    for (int val : merged) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    
    // Example 2: Merge K sorted arrays (iterative)
    std::vector<int> mergedIter = mergeKSortedArraysIterative(arrays);
    std::cout << "Merged (iterative): ";
    for (int val : mergedIter) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    
    // Example 3: External sort merger
    ExternalSortMergerDC merger;
    merger.addRun({1, 3, 5});
    merger.addRun({2, 4, 6});
    merger.addRun({7, 9, 11});
    merger.addRun({8, 10, 12});
    
    merger.mergeInBatches(2);
    std::vector<int> result = merger.getResult();
    std::cout << "External sort result: ";
    for (int val : result) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    
    return 0;
}

