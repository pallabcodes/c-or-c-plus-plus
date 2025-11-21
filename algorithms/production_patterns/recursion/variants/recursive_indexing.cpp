/*
 * Recursive Indexing Algorithm
 * 
 * Source: Compression algorithms, run-length encoding
 * Pattern: Represent large values using recursive differences
 * 
 * What Makes It Ingenious:
 * - Recursive encoding: Encode large values using smaller alphabet
 * - Run-length encoding: Encode long runs efficiently
 * - Recursive differences: Successive differences until in range
 * - Used in data compression, encoding systems
 * - Efficient for sparse or repetitive data
 * 
 * When to Use:
 * - Run-length encoding with large runs
 * - Encoding large numeric values with small alphabet
 * - Data compression
 * - Sparse data representation
 * - Repetitive pattern encoding
 * 
 * Real-World Usage:
 * - Run-length encoding systems
 * - Data compression algorithms
 * - Image compression
 * - Sparse matrix encoding
 * 
 * Time Complexity: O(log n) where n is value size
 * Space Complexity: O(log n) for encoding
 */

#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>

class RecursiveIndexing {
public:
    // Encode a large value using recursive indexing
    // Encodes value using alphabet of size alphabet_size
    // Returns sequence of indices
    static std::vector<int> encode_recursive_indexing(
        int value, int alphabet_size) {
        
        std::vector<int> result;
        encode_helper(value, alphabet_size, result);
        return result;
    }
    
    // Decode recursive indexing back to original value
    static int decode_recursive_indexing(
        const std::vector<int>& indices, int alphabet_size) {
        
        if (indices.empty()) return 0;
        
        int value = indices[0];
        
        for (size_t i = 1; i < indices.size(); i++) {
            value = value * alphabet_size + indices[i];
        }
        
        return value;
    }
    
    // Run-length encoding with recursive indexing for long runs
    static std::vector<std::pair<int, int>> run_length_encode_recursive(
        const std::vector<int>& data, int alphabet_size) {
        
        std::vector<std::pair<int, int>> encoded;
        
        if (data.empty()) return encoded;
        
        int current_value = data[0];
        int run_length = 1;
        
        for (size_t i = 1; i < data.size(); i++) {
            if (data[i] == current_value) {
                run_length++;
            } else {
                // Encode run
                if (run_length > alphabet_size) {
                    // Use recursive indexing for long runs
                    auto indices = encode_recursive_indexing(
                        run_length, alphabet_size);
                    for (int idx : indices) {
                        encoded.push_back({current_value, idx});
                    }
                } else {
                    encoded.push_back({current_value, run_length});
                }
                
                current_value = data[i];
                run_length = 1;
            }
        }
        
        // Encode last run
        if (run_length > alphabet_size) {
            auto indices = encode_recursive_indexing(run_length, alphabet_size);
            for (int idx : indices) {
                encoded.push_back({current_value, idx});
            }
        } else {
            encoded.push_back({current_value, run_length});
        }
        
        return encoded;
    }
    
    // Decode run-length encoding with recursive indexing
    static std::vector<int> run_length_decode_recursive(
        const std::vector<std::pair<int, int>>& encoded, 
        int alphabet_size) {
        
        std::vector<int> decoded;
        
        for (const auto& pair : encoded) {
            int value = pair.first;
            int length = pair.second;
            
            // Check if this is part of recursive encoding
            // (Simplified: assume single value means direct length)
            for (int i = 0; i < length; i++) {
                decoded.push_back(value);
            }
        }
        
        return decoded;
    }
    
    // Encode sparse vector using recursive indexing
    static std::vector<std::pair<int, int>> encode_sparse_vector(
        const std::vector<int>& vector, int alphabet_size) {
        
        std::vector<std::pair<int, int>> encoded;
        
        int zero_run = 0;
        
        for (size_t i = 0; i < vector.size(); i++) {
            if (vector[i] == 0) {
                zero_run++;
            } else {
                if (zero_run > 0) {
                    // Encode zero run with recursive indexing
                    if (zero_run > alphabet_size) {
                        auto indices = encode_recursive_indexing(
                            zero_run, alphabet_size);
                        for (int idx : indices) {
                            encoded.push_back({0, idx});
                        }
                    } else {
                        encoded.push_back({0, zero_run});
                    }
                    zero_run = 0;
                }
                encoded.push_back({vector[i], 1});
            }
        }
        
        // Encode final zero run if any
        if (zero_run > 0) {
            if (zero_run > alphabet_size) {
                auto indices = encode_recursive_indexing(zero_run, alphabet_size);
                for (int idx : indices) {
                    encoded.push_back({0, idx});
                }
            } else {
                encoded.push_back({0, zero_run});
            }
        }
        
        return encoded;
    }
    
private:
    static void encode_helper(int value, int alphabet_size, 
                             std::vector<int>& result) {
        if (value < alphabet_size) {
            result.push_back(value);
            return;
        }
        
        // Recursive case: encode quotient and remainder
        int quotient = value / alphabet_size;
        int remainder = value % alphabet_size;
        
        encode_helper(quotient, alphabet_size, result);
        result.push_back(remainder);
    }
    
public:
    // Fibonacci encoding using recursive indexing
    static std::vector<int> fibonacci_encode(int value) {
        // Generate Fibonacci numbers up to value
        std::vector<int> fib = {1, 2};
        while (fib.back() < value) {
            fib.push_back(fib[fib.size() - 1] + fib[fib.size() - 2]);
        }
        
        std::vector<int> encoded;
        int remaining = value;
        
        // Greedy encoding: use largest Fibonacci number possible
        for (int i = fib.size() - 1; i >= 0; i--) {
            if (fib[i] <= remaining) {
                encoded.push_back(1);
                remaining -= fib[i];
            } else {
                if (!encoded.empty()) {
                    encoded.push_back(0);
                }
            }
        }
        
        // Add terminator
        encoded.push_back(1);
        
        return encoded;
    }
    
    // Elias gamma encoding (recursive-like structure)
    static std::vector<bool> elias_gamma_encode(int value) {
        if (value <= 0) return {};
        
        std::vector<bool> encoded;
        
        // Find number of bits needed
        int bits = 0;
        int temp = value;
        while (temp > 0) {
            bits++;
            temp >>= 1;
        }
        
        // Encode: (bits-1) zeros, then binary representation
        for (int i = 0; i < bits - 1; i++) {
            encoded.push_back(false);
        }
        
        // Encode binary representation
        for (int i = bits - 1; i >= 0; i--) {
            encoded.push_back((value >> i) & 1);
        }
        
        return encoded;
    }
};

// Example usage
int main() {
    // Recursive indexing encoding
    int value = 1000;
    int alphabet_size = 10;
    
    auto encoded = RecursiveIndexing::encode_recursive_indexing(
        value, alphabet_size);
    
    std::cout << "Value: " << value << std::endl;
    std::cout << "Encoded: ";
    for (int idx : encoded) {
        std::cout << idx << " ";
    }
    std::cout << std::endl;
    
    int decoded = RecursiveIndexing::decode_recursive_indexing(
        encoded, alphabet_size);
    std::cout << "Decoded: " << decoded << std::endl;
    
    // Run-length encoding with recursive indexing
    std::vector<int> data = {1, 1, 1, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3};
    auto rle = RecursiveIndexing::run_length_encode_recursive(data, 10);
    
    std::cout << "\nRun-length encoding:" << std::endl;
    for (const auto& pair : rle) {
        std::cout << "(" << pair.first << ", " << pair.second << ") ";
    }
    std::cout << std::endl;
    
    // Fibonacci encoding
    auto fib_encoded = RecursiveIndexing::fibonacci_encode(13);
    std::cout << "\nFibonacci encoding of 13: ";
    for (int bit : fib_encoded) {
        std::cout << bit;
    }
    std::cout << std::endl;
    
    return 0;
}

