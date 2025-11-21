/*
 * Two Pointers - Opposite Ends Pattern
 * 
 * Source: Generic pattern, commonly used in production
 * 
 * What Makes It Ingenious:
 * - Eliminates half of search space per iteration
 * - O(n) time complexity for sorted arrays
 * - O(1) space complexity (in-place)
 * - Works for pairs, triplets, and n-sum problems
 * 
 * When to Use:
 * - Sorted array
 * - Need to find pairs/triplets with properties
 * - Can eliminate search space based on comparison
 * 
 * Real-World Usage:
 * - Container with most water
 * - Pair with target sum
 * - 3Sum, 4Sum problems
 * - Palindrome checking
 */

#include <vector>
#include <algorithm>
#include <iostream>

// Example 1: Pair with Target Sum
std::pair<int, int> pairWithTargetSum(const std::vector<int>& arr, int target) {
    int left = 0;
    int right = arr.size() - 1;
    
    while (left < right) {
        int sum = arr[left] + arr[right];
        
        if (sum == target) {
            return {left, right};
        } else if (sum < target) {
            left++; // Need larger sum, move left pointer right
        } else {
            right--; // Need smaller sum, move right pointer left
        }
    }
    
    return {-1, -1}; // Not found
}

// Example 2: Container With Most Water
int maxArea(const std::vector<int>& height) {
    int left = 0;
    int right = height.size() - 1;
    int max_area = 0;
    
    while (left < right) {
        // Calculate area
        int width = right - left;
        int min_height = std::min(height[left], height[right]);
        int area = width * min_height;
        max_area = std::max(max_area, area);
        
        // Move pointer pointing to smaller height
        // This is the key insight: we can eliminate the smaller height
        // because width decreases, so area can only decrease
        if (height[left] < height[right]) {
            left++;
        } else {
            right--;
        }
    }
    
    return max_area;
}

// Example 3: 3Sum (Find all unique triplets that sum to zero)
std::vector<std::vector<int>> threeSum(std::vector<int>& nums) {
    std::vector<std::vector<int>> result;
    
    // Sort first (required for two pointers)
    std::sort(nums.begin(), nums.end());
    
    for (int i = 0; i < nums.size() - 2; i++) {
        // Skip duplicates
        if (i > 0 && nums[i] == nums[i - 1]) {
            continue;
        }
        
        // Two pointers for remaining array
        int left = i + 1;
        int right = nums.size() - 1;
        int target = -nums[i]; // We want nums[left] + nums[right] = -nums[i]
        
        while (left < right) {
            int sum = nums[left] + nums[right];
            
            if (sum == target) {
                result.push_back({nums[i], nums[left], nums[right]});
                
                // Skip duplicates
                while (left < right && nums[left] == nums[left + 1]) left++;
                while (left < right && nums[right] == nums[right - 1]) right--;
                
                left++;
                right--;
            } else if (sum < target) {
                left++;
            } else {
                right--;
            }
        }
    }
    
    return result;
}

// Example 4: Valid Palindrome
bool isPalindrome(const std::string& s) {
    int left = 0;
    int right = s.length() - 1;
    
    while (left < right) {
        // Skip non-alphanumeric characters
        while (left < right && !std::isalnum(s[left])) {
            left++;
        }
        while (left < right && !std::isalnum(s[right])) {
            right--;
        }
        
        // Compare characters (case-insensitive)
        if (std::tolower(s[left]) != std::tolower(s[right])) {
            return false;
        }
        
        left++;
        right--;
    }
    
    return true;
}

// Example usage
int main() {
    // Example 1: Pair with target sum
    std::vector<int> arr1 = {1, 2, 3, 4, 6};
    auto pair = pairWithTargetSum(arr1, 6);
    std::cout << "Pair indices: " << pair.first << ", " << pair.second << std::endl;
    
    // Example 2: Container with most water
    std::vector<int> height = {1, 8, 6, 2, 5, 4, 8, 3, 7};
    std::cout << "Max area: " << maxArea(height) << std::endl;
    
    // Example 3: 3Sum
    std::vector<int> nums = {-1, 0, 1, 2, -1, -4};
    auto triplets = threeSum(nums);
    std::cout << "3Sum triplets: " << triplets.size() << std::endl;
    
    // Example 4: Palindrome
    std::string s = "A man, a plan, a canal: Panama";
    std::cout << "Is palindrome: " << isPalindrome(s) << std::endl;
    
    return 0;
}

