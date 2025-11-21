/*
 * React Hooks Linked List - State Management via Linked List
 * 
 * Source: https://github.com/facebook/react/blob/main/packages/react-reconciler/src/ReactFiberHooks.js
 * Repository: facebook/react
 * File: `packages/react-reconciler/src/ReactFiberHooks.js`
 * 
 * What Makes It Ingenious:
 * - Hooks stored as linked list on fiber's memoizedState field
 * - Each hook has next pointer to next hook
 * - Order matters: hooks must be called in same order every render
 * - Enables useState, useEffect, etc. to work correctly
 * - Work-in-progress hook list created during render
 * - Current hook list preserved for state persistence
 * - Used in React Hooks system for state management
 * 
 * When to Use:
 * - Need to maintain order-dependent state
 * - State management with hooks pattern
 * - Sequential processing with order preservation
 * - Work-in-progress vs current state pattern
 * - Component state management
 * 
 * Real-World Usage:
 * - React Hooks (useState, useEffect, useContext, etc.)
 * - Component state management
 * - Effect management
 * - Custom hooks
 * 
 * Time Complexity:
 * - Add hook: O(1) at end
 * - Traverse hooks: O(n) where n is number of hooks
 * - Find hook: O(n) worst case
 * 
 * Space Complexity: O(n) for hook list
 */

#include <cstdint>
#include <functional>
#include <any>

// Hook types
enum HookType {
    State,
    Effect,
    Context,
    Ref,
    Memo,
    Callback
};

// Base hook structure (simplified from React)
struct Hook {
    HookType type;
    Hook* next;  // Next hook in linked list
    
    // State data (varies by hook type)
    std::any memoized_state;  // Current state value
    std::any base_state;      // Base state for updates
    std::any queue;           // Update queue
    
    // Effect-specific fields
    std::function<void()> effect_cleanup;
    std::function<void()> effect_callback;
    int effect_deps_hash;
    
    Hook(HookType t) 
        : type(t)
        , next(nullptr)
        , effect_deps_hash(0) {}
    
    virtual ~Hook() {}
};

// State hook (useState)
struct StateHook : public Hook {
    StateHook() : Hook(State) {}
};

// Effect hook (useEffect)
struct EffectHook : public Hook {
    EffectHook() : Hook(Effect) {}
};

// Hook list manager (simplified from React)
class ReactHooksList {
private:
    Hook* current_hooks_;      // Current hooks (from last render)
    Hook* work_in_progress_hooks_; // WIP hooks (current render)
    Hook* last_wip_hook_;     // Last hook in WIP list
    
    int hook_index_;           // Current hook index (for order checking)
    
    // Clone hook from current to WIP
    Hook* clone_hook(Hook* current_hook) {
        if (current_hook == nullptr) return nullptr;
        
        Hook* cloned = nullptr;
        switch (current_hook->type) {
            case State:
                cloned = new StateHook();
                cloned->memoized_state = current_hook->memoized_state;
                cloned->base_state = current_hook->base_state;
                cloned->queue = current_hook->queue;
                break;
            case Effect:
                cloned = new EffectHook();
                cloned->effect_callback = current_hook->effect_callback;
                cloned->effect_cleanup = current_hook->effect_cleanup;
                cloned->effect_deps_hash = current_hook->effect_deps_hash;
                break;
            default:
                cloned = new Hook(current_hook->type);
                cloned->memoized_state = current_hook->memoized_state;
                break;
        }
        return cloned;
    }
    
public:
    ReactHooksList() 
        : current_hooks_(nullptr)
        , work_in_progress_hooks_(nullptr)
        , last_wip_hook_(nullptr)
        , hook_index_(0) {}
    
    // Begin render (create WIP hook list from current)
    void begin_render() {
        work_in_progress_hooks_ = nullptr;
        last_wip_hook_ = nullptr;
        hook_index_ = 0;
        
        // Clone current hooks to WIP
        Hook* current = current_hooks_;
        Hook* prev_wip = nullptr;
        
        while (current != nullptr) {
            Hook* cloned = clone_hook(current);
            
            if (work_in_progress_hooks_ == nullptr) {
                work_in_progress_hooks_ = last_wip_hook_ = cloned;
            } else {
                prev_wip->next = cloned;
                last_wip_hook_ = cloned;
            }
            
            prev_wip = cloned;
            current = current->next;
        }
    }
    
    // Get next hook (React's pattern - order matters!)
    Hook* get_next_hook() {
        Hook* hook = nullptr;
        
        if (work_in_progress_hooks_ != nullptr) {
            // Get hook from WIP list
            Hook* current = work_in_progress_hooks_;
            int index = 0;
            
            while (current != nullptr && index < hook_index_) {
                current = current->next;
                index++;
            }
            
            hook = current;
        }
        
        // If no hook found, create new one
        if (hook == nullptr) {
            hook = new StateHook(); // Default to state hook
            
            if (work_in_progress_hooks_ == nullptr) {
                work_in_progress_hooks_ = last_wip_hook_ = hook;
            } else {
                last_wip_hook_->next = hook;
                last_wip_hook_ = hook;
            }
        }
        
        hook_index_++;
        return hook;
    }
    
    // Commit render (replace current with WIP)
    void commit_render() {
        // Cleanup old current hooks
        Hook* current = current_hooks_;
        while (current != nullptr) {
            Hook* next = current->next;
            delete current;
            current = next;
        }
        
        // Replace with WIP
        current_hooks_ = work_in_progress_hooks_;
        work_in_progress_hooks_ = nullptr;
        last_wip_hook_ = nullptr;
        hook_index_ = 0;
    }
    
    // Traverse current hooks
    void traverse_hooks(std::function<void(Hook*)> visit) {
        Hook* current = current_hooks_;
        while (current != nullptr) {
            visit(current);
            current = current->next;
        }
    }
    
    // Traverse WIP hooks
    void traverse_wip_hooks(std::function<void(Hook*)> visit) {
        Hook* current = work_in_progress_hooks_;
        while (current != nullptr) {
            visit(current);
            current = current->next;
        }
    }
    
    // Get hook count
    int get_hook_count() const {
        int count = 0;
        Hook* current = current_hooks_;
        while (current != nullptr) {
            count++;
            current = current->next;
        }
        return count;
    }
};

// Example usage (simulating useState)
#include <iostream>

template<typename T>
T useState(ReactHooksList& hooks_list, T initial_value) {
    Hook* hook = hooks_list.get_next_hook();
    
    if (hook->type != State) {
        // Error: wrong hook type
        return initial_value;
    }
    
    // Initialize if first time
    if (hook->memoized_state.type() == typeid(void)) {
        hook->memoized_state = initial_value;
    }
    
    // Return current state (simplified - real React has more logic)
    return std::any_cast<T>(hook->memoized_state);
}

int main() {
    ReactHooksList hooks;
    
    // Simulate component render
    std::cout << "First render:" << std::endl;
    hooks.begin_render();
    
    int count1 = useState(hooks, 0);
    int count2 = useState(hooks, 10);
    
    std::cout << "Hook 1: " << count1 << std::endl;
    std::cout << "Hook 2: " << count2 << std::endl;
    
    hooks.commit_render();
    
    // Second render (hooks preserved)
    std::cout << "\nSecond render:" << std::endl;
    hooks.begin_render();
    
    int count1_again = useState(hooks, 0);
    int count2_again = useState(hooks, 10);
    
    std::cout << "Hook 1: " << count1_again << std::endl;
    std::cout << "Hook 2: " << count2_again << std::endl;
    
    std::cout << "\nTotal hooks: " << hooks.get_hook_count() << std::endl;
    
    hooks.commit_render();
    
    return 0;
}

