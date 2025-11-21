// Mo's Algorithm: Offline range queries
// Based on competitive programming techniques
// Time: O((n + q) * sqrt(n)) for q queries
// Space: O(n + q)
// God modded implementation for efficient range queries

#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>

struct Query {
    int left;
    int right;
    int index;
    
    Query(int l, int r, int idx) : left(l), right(r), index(idx) {}
};

class MosAlgorithm {
private:
    std::vector<int> arr;
    std::vector<long long> answers;
    int currentAnswer;
    int currentLeft;
    int currentRight;
    
    void add(int position) {
        currentAnswer += arr[position];
    }
    
    void remove(int position) {
        currentAnswer -= arr[position];
    }
    
public:
    MosAlgorithm(const std::vector<int>& array) 
        : arr(array), currentAnswer(0), currentLeft(0), currentRight(-1) {}
    
    std::vector<long long> processQueries(std::vector<Query>& queries) {
        int n = arr.size();
        int blockSize = sqrt(n);
        
        std::sort(queries.begin(), queries.end(), 
                 [blockSize](const Query& a, const Query& b) {
            int blockA = a.left / blockSize;
            int blockB = b.left / blockSize;
            
            if (blockA != blockB) {
                return blockA < blockB;
            }
            return (blockA % 2 == 0) ? a.right < b.right : a.right > b.right;
        });
        
        answers.resize(queries.size());
        currentLeft = 0;
        currentRight = -1;
        currentAnswer = 0;
        
        for (const Query& q : queries) {
            while (currentLeft > q.left) {
                currentLeft--;
                add(currentLeft);
            }
            while (currentRight < q.right) {
                currentRight++;
                add(currentRight);
            }
            while (currentLeft < q.left) {
                remove(currentLeft);
                currentLeft++;
            }
            while (currentRight > q.right) {
                remove(currentRight);
                currentRight--;
            }
            
            answers[q.index] = currentAnswer;
        }
        
        return answers;
    }
};

// Mo's algorithm for distinct element count
class DistinctElementsMo {
private:
    std::vector<int> arr;
    std::vector<int> freq;
    int distinctCount;
    
    void add(int position) {
        freq[arr[position]]++;
        if (freq[arr[position]] == 1) {
            distinctCount++;
        }
    }
    
    void remove(int position) {
        freq[arr[position]]--;
        if (freq[arr[position]] == 0) {
            distinctCount--;
        }
    }
    
public:
    DistinctElementsMo(const std::vector<int>& array, int maxVal) 
        : arr(array), freq(maxVal + 1, 0), distinctCount(0) {}
    
    std::vector<int> processQueries(std::vector<Query>& queries) {
        int n = arr.size();
        int blockSize = sqrt(n);
        
        std::sort(queries.begin(), queries.end(), 
                 [blockSize](const Query& a, const Query& b) {
            int blockA = a.left / blockSize;
            int blockB = b.left / blockSize;
            
            if (blockA != blockB) {
                return blockA < blockB;
            }
            return (blockA % 2 == 0) ? a.right < b.right : a.right > b.right;
        });
        
        std::vector<int> answers(queries.size());
        int currentLeft = 0;
        int currentRight = -1;
        distinctCount = 0;
        
        for (const Query& q : queries) {
            while (currentLeft > q.left) {
                currentLeft--;
                add(currentLeft);
            }
            while (currentRight < q.right) {
                currentRight++;
                add(currentRight);
            }
            while (currentLeft < q.left) {
                remove(currentLeft);
                currentLeft++;
            }
            while (currentRight > q.right) {
                remove(currentRight);
                currentRight--;
            }
            
            answers[q.index] = distinctCount;
        }
        
        return answers;
    }
};

// Example usage
int main() {
    std::vector<int> arr = {1, 1, 2, 1, 3, 4, 3, 3, 4};
    
    std::vector<Query> queries = {
        Query(0, 4, 0),
        Query(1, 3, 1),
        Query(2, 4, 2),
        Query(0, 8, 3)
    };
    
    MosAlgorithm mos(arr);
    std::vector<long long> answers = mos.processQueries(queries);
    
    std::cout << "Range sum queries:" << std::endl;
    for (size_t i = 0; i < queries.size(); i++) {
        std::cout << "Query [" << queries[i].left << ", " << queries[i].right 
                  << "]: " << answers[i] << std::endl;
    }
    
    DistinctElementsMo distinctMo(arr, 10);
    std::vector<int> distinctAnswers = distinctMo.processQueries(queries);
    
    std::cout << "\nDistinct elements queries:" << std::endl;
    for (size_t i = 0; i < queries.size(); i++) {
        std::cout << "Query [" << queries[i].left << ", " << queries[i].right 
                  << "]: " << distinctAnswers[i] << " distinct elements" << std::endl;
    }
    
    return 0;
}

