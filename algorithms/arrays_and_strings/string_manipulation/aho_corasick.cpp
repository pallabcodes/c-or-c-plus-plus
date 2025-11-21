// Aho-Corasick Algorithm: Multiple pattern string matching
// Based on research paper by Aho and Corasick
// Time: O(n + m + z) where n is text length, m is total pattern length, z is matches
// Space: O(m)
// God modded implementation with failure links and output links

#include <vector>
#include <string>
#include <queue>
#include <iostream>
#include <map>
#include <set>

class AhoCorasick {
private:
    struct Node {
        std::map<char, int> children;
        int fail;
        std::vector<int> output;
        bool isEnd;
        
        Node() : fail(-1), isEnd(false) {}
    };
    
    std::vector<Node> trie;
    int nodeCount;
    
    void buildFailureLinks() {
        std::queue<int> q;
        
        for (auto& pair : trie[0].children) {
            int child = pair.second;
            trie[child].fail = 0;
            q.push(child);
        }
        
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            
            for (auto& pair : trie[u].children) {
                char c = pair.first;
                int v = pair.second;
                
                int fail = trie[u].fail;
                while (fail != -1 && trie[fail].children.find(c) == trie[fail].children.end()) {
                    fail = trie[fail].fail;
                }
                
                if (fail == -1) {
                    trie[v].fail = 0;
                } else {
                    trie[v].fail = trie[fail].children[c];
                }
                
                trie[v].output = trie[trie[v].fail].output;
                if (trie[v].isEnd) {
                    trie[v].output.push_back(v);
                }
                
                q.push(v);
            }
        }
    }
    
public:
    AhoCorasick() : nodeCount(1) {
        trie.push_back(Node());
    }
    
    void addPattern(const std::string& pattern, int patternId) {
        int node = 0;
        
        for (char c : pattern) {
            if (trie[node].children.find(c) == trie[node].children.end()) {
                trie[node].children[c] = nodeCount++;
                trie.push_back(Node());
            }
            node = trie[node].children[c];
        }
        
        trie[node].isEnd = true;
    }
    
    void build() {
        buildFailureLinks();
    }
    
    std::vector<std::pair<int, int>> search(const std::string& text) {
        std::vector<std::pair<int, int>> matches;
        int node = 0;
        
        for (size_t i = 0; i < text.length(); i++) {
            char c = text[i];
            
            while (node != -1 && trie[node].children.find(c) == trie[node].children.end()) {
                node = trie[node].fail;
            }
            
            if (node == -1) {
                node = 0;
                continue;
            }
            
            node = trie[node].children[c];
            
            for (int outputNode : trie[node].output) {
                matches.push_back({i, outputNode});
            }
        }
        
        return matches;
    }
    
    std::vector<std::pair<int, std::string>> searchWithPatterns(const std::string& text, 
                                                                 const std::vector<std::string>& patterns) {
        std::vector<std::pair<int, int>> matches = search(text);
        std::vector<std::pair<int, std::string>> result;
        
        for (const auto& match : matches) {
            int pos = match.first;
            int nodeId = match.second;
            
            for (size_t i = 0; i < patterns.size(); i++) {
                if (trie[nodeId].isEnd) {
                    int patternStart = pos - patterns[i].length() + 1;
                    if (patternStart >= 0) {
                        result.push_back({patternStart, patterns[i]});
                    }
                }
            }
        }
        
        return result;
    }
};

// Example usage
int main() {
    AhoCorasick ac;
    
    std::vector<std::string> patterns = {"he", "she", "his", "hers"};
    
    for (size_t i = 0; i < patterns.size(); i++) {
        ac.addPattern(patterns[i], i);
    }
    
    ac.build();
    
    std::string text = "ushers";
    
    std::cout << "Searching for patterns in: " << text << std::endl;
    std::cout << "Patterns: ";
    for (const auto& p : patterns) {
        std::cout << p << " ";
    }
    std::cout << std::endl;
    
    auto matches = ac.searchWithPatterns(text, patterns);
    
    std::cout << "Matches found:" << std::endl;
    for (const auto& match : matches) {
        std::cout << "  Position " << match.first << ": " << match.second << std::endl;
    }
    
    return 0;
}

