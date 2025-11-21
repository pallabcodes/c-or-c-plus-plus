/*
 * Two Pointers - Same Direction Pattern
 * 
 * Source: Generic pattern, commonly used in production
 * 
 * What Makes It Ingenious:
 * - In-place modification (O(1) space)
 * - Single pass through array (O(n) time)
 * - Maintains relative order
 * - Works for filtering, removing, partitioning
 * 
 * When to Use:
 * - Remove duplicates from sorted array
 * - Remove specific element
 * - Move zeros to end
 * - Partition array based on condition
 * - In-place modifications
 * 
 * Real-World Usage:
 * - Data deduplication
 * - Array filtering
 * - In-place sorting/partitioning
 * - Memory-efficient array operations
 */

#include <vector>
#include <algorithm>
#include <iostream>

// Example 1: Remove Duplicates from Sorted Array
int removeDuplicates(std::vector<int>& nums) {
    if (nums.empty()) {
        return 0;
    }
    
    int slow = 0; // Points to last unique element
    
    // Fast pointer scans through array
    for (int fast = 1; fast < nums.size(); fast++) {
        // If current element is different from last unique
        if (nums[fast] != nums[slow]) {
            slow++;
            nums[slow] = nums[fast]; // Place unique element
        }
    }
    
    return slow + 1; // Return new length
}

// Example 2: Remove Element (remove all occurrences of val)
int removeElement(std::vector<int>& nums, int val) {
    int slow = 0; // Points to next position to write
    
    for (int fast = 0; fast < nums.size(); fast++) {
        // If current element is not val, keep it
        if (nums[fast] != val) {
            nums[slow] = nums[fast];
            slow++;
        }
        // If nums[fast] == val, skip it (don't increment slow)
    }
    
    return slow; // Return new length
}

// Example 3: Move Zeros to End
void moveZeros(std::vector<int>& nums) {
    int slow = 0; // Points to next position for non-zero element
    
    // First pass: move all non-zeros to front
    for (int fast = 0; fast < nums.size(); fast++) {
        if (nums[fast] != 0) {
            nums[slow] = nums[fast];
            slow++;
        }
    }
    
    // Second pass: fill remaining with zeros
    for (int i = slow; i < nums.size(); i++) {
        nums[i] = 0;
    }
}

// Example 4: Move Zeros to End (Optimized - Single Pass)
void moveZerosOptimized(std::vector<int>& nums) {
    int slow = 0;
    
    for (int fast = 0; fast < nums.size(); fast++) {
        if (nums[fast] != 0) {
            // Swap if needed (only swap when slow != fast)
            if (slow != fast) {
                std::swap(nums[slow], nums[fast]);
            }
            slow++;
        }
    }
}

// Example 5: Remove Duplicates (Allow at most 2 duplicates)
int removeDuplicatesAtMostTwo(std::vector<int>& nums) {
    if (nums.size() <= 2) {
        return nums.size();
    }
    
    int slow = 1; // Points to next position to write
    
    for (int fast = 2; fast < nums.size(); fast++) {
        // Keep element if it's different from slow-1 (allows 2 same)
        // Or if it's same as slow-1 but different from slow-2
        if (nums[fast] != nums[slow - 1]) {
            slow++;
            nums[slow] = nums[fast];
        }
    }
    
    return slow + 1;
}

// Example 6: Partition Array (Move elements < pivot to left)
int partition(std::vector<int>& nums, int pivot) {
    int slow = 0; // Points to next position for element < pivot
    
    for (int fast = 0; fast < nums.size(); fast++) {
        if (nums[fast] < pivot) {
            std::swap(nums[slow], nums[fast]);
            slow++;
        }
    }
    
    return slow; // Return partition point
}

// Example 7: Sort Colors (Dutch National Flag - 3-way partition)
void sortColors(std::vector<int>& nums) {
    int left = 0;      // Points to next position for 0
    int right = nums.size() - 1; // Points to next position for 2
    int curr = 0;      // Current pointer
    
    while (curr <= right) {
        if (nums[curr] == 0) {
            // Move 0 to left
            std::swap(nums[left], nums[curr]);
            left++;
            curr++;
        } else if (nums[curr] == 2) {
            // Move 2 to right
            std::swap(nums[curr], nums[right]);
            right--;
            // Don't increment curr - need to check swapped element
        } else {
            // nums[curr] == 1, keep it in place
            curr++;
        }
    }
}

// Example 8: Squaring Sorted Array (with negatives)
std::vector<int> sortedSquares(const std::vector<int>& nums) {
    int n = nums.size();
    std::vector<int> result(n);
    
    // Two pointers from both ends
    int left = 0;
    int right = n - 1;
    int pos = n - 1; // Fill from end (largest squares)
    
    while (left <= right) {
        int leftSquare = nums[left] * nums[left];
        int rightSquare = nums[right] * nums[right];
        
        if (leftSquare > rightSquare) {
            result[pos] = leftSquare;
            left++;
        } else {
            result[pos] = rightSquare;
            right--;
        }
        pos--;
    }
    
    return result;
}

// Example usage
int main() {
    // Example 1: Remove duplicates
    std::vector<int> nums1 = {1, 1, 2, 2, 3, 4, 4, 5};
    int len1 = removeDuplicates(nums1);
    std::cout << "After removing duplicates: ";
    for (int i = 0; i < len1; i++) {
        std::cout << nums1[i] << " ";
    }
    std::cout << std::endl;
    
    // Example 2: Remove element
    std::vector<int> nums2 = {3, 2, 2, 3};
    int len2 = removeElement(nums2, 3);
    std::cout << "After removing 3: ";
    for (int i = 0; i < len2; i++) {
        std::cout << nums2[i] << " ";
    }
    std::cout << std::endl;
    
    // Example 3: Move zeros
    std::vector<int> nums3 = {0, 1, 0, 3, 12};
    moveZerosOptimized(nums3);
    std::cout << "After moving zeros: ";
    for (int num : nums3) {
        std::cout << num << " ";
    }
    std::cout << std::endl;
    
    // Example 4: Sort colors
    std::vector<int> colors = {2, 0, 2, 1, 1, 0};
    sortColors(colors);
    std::cout << "After sorting colors: ";
    for (int color : colors) {
        std::cout << color << " ";
    }
    std::cout << std::endl;
    
    return 0;
}

