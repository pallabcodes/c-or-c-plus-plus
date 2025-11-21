// Fast Walsh-Hadamard Transform (FWT)
// Used for convolution over XOR, AND, OR operations
// Based on research in signal processing and competitive programming
// Time: O(n log n)
// Space: O(n)
// God modded implementation for bitwise convolution

#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>

// FWT for XOR convolution
void fwtXOR(std::vector<long long>& a, bool inverse) {
    int n = a.size();
    
    for (int len = 1; 2 * len <= n; len <<= 1) {
        for (int i = 0; i < n; i += 2 * len) {
            for (int j = 0; j < len; j++) {
                long long u = a[i + j];
                long long v = a[i + j + len];
                
                a[i + j] = u + v;
                a[i + j + len] = u - v;
            }
        }
    }
    
    if (inverse) {
        for (int i = 0; i < n; i++) {
            a[i] /= n;
        }
    }
}

// FWT for AND convolution
void fwtAND(std::vector<long long>& a, bool inverse) {
    int n = a.size();
    
    for (int len = 1; 2 * len <= n; len <<= 1) {
        for (int i = 0; i < n; i += 2 * len) {
            for (int j = 0; j < len; j++) {
                if (inverse) {
                    a[i + j] -= a[i + j + len];
                } else {
                    a[i + j] += a[i + j + len];
                }
            }
        }
    }
}

// FWT for OR convolution
void fwtOR(std::vector<long long>& a, bool inverse) {
    int n = a.size();
    
    for (int len = 1; 2 * len <= n; len <<= 1) {
        for (int i = 0; i < n; i += 2 * len) {
            for (int j = 0; j < len; j++) {
                if (inverse) {
                    a[i + j + len] -= a[i + j];
                } else {
                    a[i + j + len] += a[i + j];
                }
            }
        }
    }
}

// Convolution using FWT
std::vector<long long> convolveXOR(const std::vector<long long>& a, 
                                    const std::vector<long long>& b) {
    int n = a.size();
    std::vector<long long> fa = a, fb = b;
    
    fwtXOR(fa, false);
    fwtXOR(fb, false);
    
    for (int i = 0; i < n; i++) {
        fa[i] *= fb[i];
    }
    
    fwtXOR(fa, true);
    
    return fa;
}

std::vector<long long> convolveAND(const std::vector<long long>& a, 
                                    const std::vector<long long>& b) {
    int n = a.size();
    std::vector<long long> fa = a, fb = b;
    
    fwtAND(fa, false);
    fwtAND(fb, false);
    
    for (int i = 0; i < n; i++) {
        fa[i] *= fb[i];
    }
    
    fwtAND(fa, true);
    
    return fa;
}

std::vector<long long> convolveOR(const std::vector<long long>& a, 
                                   const std::vector<long long>& b) {
    int n = a.size();
    std::vector<long long> fa = a, fb = b;
    
    fwtOR(fa, false);
    fwtOR(fb, false);
    
    for (int i = 0; i < n; i++) {
        fa[i] *= fb[i];
    }
    
    fwtOR(fa, true);
    
    return fa;
}

// Example usage
int main() {
    int n = 8;
    std::vector<long long> a = {1, 2, 3, 4, 5, 6, 7, 8};
    std::vector<long long> b = {1, 1, 1, 1, 1, 1, 1, 1};
    
    std::cout << "Array A: ";
    for (long long x : a) std::cout << x << " ";
    std::cout << std::endl;
    
    std::cout << "Array B: ";
    for (long long x : b) std::cout << x << " ";
    std::cout << std::endl;
    
    auto xorConv = convolveXOR(a, b);
    std::cout << "\nXOR Convolution: ";
    for (long long x : xorConv) std::cout << x << " ";
    std::cout << std::endl;
    
    auto andConv = convolveAND(a, b);
    std::cout << "AND Convolution: ";
    for (long long x : andConv) std::cout << x << " ";
    std::cout << std::endl;
    
    auto orConv = convolveOR(a, b);
    std::cout << "OR Convolution: ";
    for (long long x : orConv) std::cout << x << " ";
    std::cout << std::endl;
    
    return 0;
}

