/*
 * Two Pointers - Fast/Slow Pattern (Floyd's Cycle Detection)
 * 
 * Source: Floyd's cycle detection algorithm, used in production
 * 
 * What Makes It Ingenious:
 * - Detects cycles in O(n) time, O(1) space
 * - No extra data structures needed
 * - Works for linked lists, arrays, and graphs
 * - Can find cycle start and length
 * 
 * When to Use:
 * - Linked list cycle detection
 * - Find middle of linked list
 * - Find nth node from end
 * - Detect cycles in arrays/graphs
 * 
 * Real-World Usage:
 * - Memory leak detection (circular references)
 * - Cycle detection in graphs
 * - Finding middle of list
 * - Palindrome checking in linked lists
 */

#include <iostream>

// Linked list node structure
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

// Example 1: Detect Cycle in Linked List
bool hasCycle(ListNode* head) {
    if (!head || !head->next) {
        return false;
    }
    
    ListNode* slow = head;
    ListNode* fast = head->next;
    
    while (fast && fast->next) {
        if (slow == fast) {
            return true; // Cycle detected
        }
        slow = slow->next;      // Move 1 step
        fast = fast->next->next; // Move 2 steps
    }
    
    return false; // No cycle
}

// Example 2: Find Cycle Start (if cycle exists)
ListNode* detectCycleStart(ListNode* head) {
    if (!head || !head->next) {
        return nullptr;
    }
    
    // Step 1: Detect if cycle exists
    ListNode* slow = head;
    ListNode* fast = head;
    
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        
        if (slow == fast) {
            break; // Cycle detected
        }
    }
    
    // No cycle
    if (!fast || !fast->next) {
        return nullptr;
    }
    
    // Step 2: Find cycle start
    // Key insight: When slow and fast meet, the distance from head
    // to cycle start equals the distance from meeting point to cycle start
    slow = head;
    while (slow != fast) {
        slow = slow->next;
        fast = fast->next;
    }
    
    return slow; // Cycle start
}

// Example 3: Find Middle of Linked List
ListNode* findMiddle(ListNode* head) {
    if (!head) {
        return nullptr;
    }
    
    ListNode* slow = head;
    ListNode* fast = head;
    
    // Fast moves 2x speed, slow moves 1x speed
    // When fast reaches end, slow is at middle
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    
    return slow;
}

// Example 4: Find Nth Node from End
ListNode* findNthFromEnd(ListNode* head, int n) {
    if (!head) {
        return nullptr;
    }
    
    // Move fast pointer n steps ahead
    ListNode* fast = head;
    for (int i = 0; i < n; i++) {
        if (!fast) {
            return nullptr; // List shorter than n
        }
        fast = fast->next;
    }
    
    // Move both pointers until fast reaches end
    ListNode* slow = head;
    while (fast) {
        slow = slow->next;
        fast = fast->next;
    }
    
    return slow; // slow is now n nodes from end
}

// Example 5: Remove Nth Node from End
ListNode* removeNthFromEnd(ListNode* head, int n) {
    // Dummy node to handle edge case (removing head)
    ListNode* dummy = new ListNode(0);
    dummy->next = head;
    
    ListNode* fast = dummy;
    ListNode* slow = dummy;
    
    // Move fast n+1 steps ahead
    for (int i = 0; i <= n; i++) {
        fast = fast->next;
    }
    
    // Move both until fast reaches end
    while (fast) {
        slow = slow->next;
        fast = fast->next;
    }
    
    // Remove node
    ListNode* toDelete = slow->next;
    slow->next = slow->next->next;
    delete toDelete;
    
    ListNode* result = dummy->next;
    delete dummy;
    return result;
}

// Example 6: Palindrome Linked List (using fast/slow)
bool isPalindromeLinkedList(ListNode* head) {
    if (!head || !head->next) {
        return true;
    }
    
    // Step 1: Find middle using fast/slow
    ListNode* slow = head;
    ListNode* fast = head;
    
    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    
    // Step 2: Reverse second half
    ListNode* secondHalf = slow->next;
    slow->next = nullptr;
    
    ListNode* prev = nullptr;
    ListNode* curr = secondHalf;
    while (curr) {
        ListNode* next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    
    // Step 3: Compare first half and reversed second half
    ListNode* firstHalf = head;
    ListNode* reversedSecondHalf = prev;
    
    while (reversedSecondHalf) {
        if (firstHalf->val != reversedSecondHalf->val) {
            return false;
        }
        firstHalf = firstHalf->next;
        reversedSecondHalf = reversedSecondHalf->next;
    }
    
    return true;
}

// Helper function to create linked list
ListNode* createList(const std::vector<int>& vals) {
    if (vals.empty()) return nullptr;
    
    ListNode* head = new ListNode(vals[0]);
    ListNode* curr = head;
    
    for (size_t i = 1; i < vals.size(); i++) {
        curr->next = new ListNode(vals[i]);
        curr = curr->next;
    }
    
    return head;
}

// Example usage
int main() {
    // Example 1: Cycle detection
    ListNode* head1 = createList({1, 2, 3, 4, 5});
    head1->next->next->next->next->next = head1->next; // Create cycle
    std::cout << "Has cycle: " << hasCycle(head1) << std::endl;
    
    // Example 2: Find middle
    ListNode* head2 = createList({1, 2, 3, 4, 5});
    ListNode* middle = findMiddle(head2);
    std::cout << "Middle value: " << middle->val << std::endl;
    
    // Example 3: Find nth from end
    ListNode* head3 = createList({1, 2, 3, 4, 5});
    ListNode* nth = findNthFromEnd(head3, 2);
    std::cout << "2nd from end: " << nth->val << std::endl;
    
    // Example 4: Palindrome check
    ListNode* head4 = createList({1, 2, 2, 1});
    std::cout << "Is palindrome: " << isPalindromeLinkedList(head4) << std::endl;
    
    return 0;
}

