// Patience Sort: Card game inspired sorting algorithm
// Based on patience card game, finds longest increasing subsequence
// Time: O(n log n) worst case
// Space: O(n)
// Ingenious algorithm that simultaneously sorts and finds LIS

#include <vector>
#include <iostream>
#include <algorithm>
#include <stack>

// Patience sort that returns sorted array
void patienceSort(std::vector<int>& arr) {
    if (arr.empty()) return;
    
    std::vector<std::vector<int>> piles;
    
    for (int card : arr) {
        bool placed = false;
        
        // Binary search for leftmost pile where top >= card
        int left = 0, right = piles.size();
        while (left < right) {
            int mid = left + (right - left) / 2;
            if (piles[mid].back() >= card) {
                right = mid;
            } else {
                left = mid + 1;
            }
        }
        
        if (left == piles.size()) {
            piles.push_back(std::vector<int>());
        }
        
        piles[left].push_back(card);
    }
    
    // Merge piles back into sorted array
    int idx = 0;
    while (!piles.empty()) {
        int minIdx = 0;
        for (size_t i = 1; i < piles.size(); i++) {
            if (piles[i].back() < piles[minIdx].back()) {
                minIdx = i;
            }
        }
        
        arr[idx++] = piles[minIdx].back();
        piles[minIdx].pop_back();
        
        if (piles[minIdx].empty()) {
            piles.erase(piles.begin() + minIdx);
        }
    }
}

// Find longest increasing subsequence using patience sort
std::vector<int> longestIncreasingSubsequence(const std::vector<int>& arr) {
    if (arr.empty()) return {};
    
    std::vector<std::pair<int, int>> piles; // (top value, index in original array)
    std::vector<int> parent(arr.size(), -1);
    
    for (size_t i = 0; i < arr.size(); i++) {
        int card = arr[i];
        
        // Binary search for leftmost pile where top >= card
        int left = 0, right = piles.size();
        while (left < right) {
            int mid = left + (right - left) / 2;
            if (piles[mid].first >= card) {
                right = mid;
            } else {
                left = mid + 1;
            }
        }
        
        if (left == piles.size()) {
            piles.push_back({card, i});
        } else {
            piles[left] = {card, i};
        }
        
        if (left > 0) {
            parent[i] = piles[left - 1].second;
        }
    }
    
    // Reconstruct LIS
    std::vector<int> lis;
    int idx = piles.back().second;
    while (idx != -1) {
        lis.push_back(arr[idx]);
        idx = parent[idx];
    }
    
    std::reverse(lis.begin(), lis.end());
    return lis;
}

// Example usage
int main() {
    std::vector<int> arr = {64, 34, 25, 12, 22, 11, 90, 5, 77, 1};
    
    std::cout << "Original array: ";
    for (int x : arr) std::cout << x << " ";
    std::cout << std::endl;
    
    patienceSort(arr);
    
    std::cout << "Sorted array: ";
    for (int x : arr) std::cout << x << " ";
    std::cout << std::endl;
    
    std::vector<int> test = {10, 9, 2, 5, 3, 7, 101, 18};
    std::cout << "\nFinding LIS of: ";
    for (int x : test) std::cout << x << " ";
    std::cout << std::endl;
    
    std::vector<int> lis = longestIncreasingSubsequence(test);
    std::cout << "Longest Increasing Subsequence: ";
    for (int x : lis) std::cout << x << " ";
    std::cout << std::endl;
    
    return 0;
}

