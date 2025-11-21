// Suffix Automaton: Linear time suffix automaton construction
// Based on research by Blumer et al. and Ukkonen
// Time: O(n) construction, O(m) pattern matching
// Space: O(n)
// God modded implementation for efficient substring queries

#include <vector>
#include <string>
#include <map>
#include <iostream>

class SuffixAutomaton {
private:
    struct State {
        int len;
        int link;
        std::map<char, int> next;
        bool isClone;
        
        State() : len(0), link(-1), isClone(false) {}
    };
    
    std::vector<State> states;
    int last;
    int size;
    
    void extend(char c) {
        int cur = size++;
        states.push_back(State());
        states[cur].len = states[last].len + 1;
        
        int p = last;
        while (p != -1 && states[p].next.find(c) == states[p].next.end()) {
            states[p].next[c] = cur;
            p = states[p].link;
        }
        
        if (p == -1) {
            states[cur].link = 0;
        } else {
            int q = states[p].next[c];
            if (states[p].len + 1 == states[q].len) {
                states[cur].link = q;
            } else {
                int clone = size++;
                states.push_back(states[q]);
                states[clone].len = states[p].len + 1;
                states[clone].isClone = true;
                states[q].link = clone;
                
                while (p != -1 && states[p].next[c] == q) {
                    states[p].next[c] = clone;
                    p = states[p].link;
                }
                
                states[cur].link = clone;
            }
        }
        
        last = cur;
    }
    
public:
    SuffixAutomaton(const std::string& s) : last(0), size(1) {
        states.push_back(State());
        
        for (char c : s) {
            extend(c);
        }
    }
    
    bool contains(const std::string& pattern) {
        int state = 0;
        
        for (char c : pattern) {
            if (states[state].next.find(c) == states[state].next.end()) {
                return false;
            }
            state = states[state].next[c];
        }
        
        return true;
    }
    
    int countOccurrences(const std::string& pattern) {
        if (!contains(pattern)) return 0;
        
        int state = 0;
        for (char c : pattern) {
            state = states[state].next[c];
        }
        
        return countEndPositions(state);
    }
    
private:
    int countEndPositions(int state) {
        int count = 1;
        
        for (const auto& pair : states[state].next) {
            count += countEndPositions(pair.second);
        }
        
        return count;
    }
    
public:
    std::string longestCommonSubstring(const std::string& other) {
        int maxLen = 0;
        int maxEnd = 0;
        int curLen = 0;
        int state = 0;
        
        for (size_t i = 0; i < other.length(); i++) {
            char c = other[i];
            
            while (state != 0 && states[state].next.find(c) == states[state].next.end()) {
                state = states[state].link;
                if (state != -1) {
                    curLen = states[state].len;
                }
            }
            
            if (states[state].next.find(c) != states[state].next.end()) {
                state = states[state].next[c];
                curLen++;
            } else {
                curLen = 0;
            }
            
            if (curLen > maxLen) {
                maxLen = curLen;
                maxEnd = i;
            }
        }
        
        if (maxLen == 0) return "";
        return other.substr(maxEnd - maxLen + 1, maxLen);
    }
};

// Example usage
int main() {
    std::string text = "banana";
    SuffixAutomaton sa(text);
    
    std::cout << "Built suffix automaton for: " << text << std::endl;
    
    std::vector<std::string> patterns = {"ana", "nan", "ban", "xyz"};
    
    for (const std::string& pattern : patterns) {
        bool found = sa.contains(pattern);
        std::cout << "Pattern \"" << pattern << "\": " 
                  << (found ? "Found" : "Not found") << std::endl;
    }
    
    std::string other = "anana";
    std::string lcs = sa.longestCommonSubstring(other);
    std::cout << "\nLongest common substring with \"" << other << "\": " << lcs << std::endl;
    
    return 0;
}

