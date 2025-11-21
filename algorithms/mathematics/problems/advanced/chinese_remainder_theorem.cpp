// Chinese Remainder Theorem (CRT)
// Solves system of congruences: x ≡ a_i (mod m_i)
// Based on number theory research
// Time: O(n log(max(m_i)))
// Space: O(n)
// God modded implementation with extended Euclidean algorithm

#include <vector>
#include <iostream>
#include <algorithm>

long long extendedGCD(long long a, long long b, long long& x, long long& y) {
    if (b == 0) {
        x = 1;
        y = 0;
        return a;
    }
    
    long long x1, y1;
    long long gcd = extendedGCD(b, a % b, x1, y1);
    
    x = y1;
    y = x1 - (a / b) * y1;
    
    return gcd;
}

long long modInverse(long long a, long long m) {
    long long x, y;
    long long gcd = extendedGCD(a, m, x, y);
    
    if (gcd != 1) {
        return -1;
    }
    
    return (x % m + m) % m;
}

// Chinese Remainder Theorem
// Solves: x ≡ a[i] (mod m[i]) for all i
long long chineseRemainderTheorem(const std::vector<long long>& a, 
                                  const std::vector<long long>& m) {
    int n = a.size();
    
    long long M = 1;
    for (int i = 0; i < n; i++) {
        M *= m[i];
    }
    
    long long result = 0;
    
    for (int i = 0; i < n; i++) {
        long long Mi = M / m[i];
        long long inv = modInverse(Mi, m[i]);
        
        if (inv == -1) {
            return -1;
        }
        
        result = (result + a[i] * Mi * inv) % M;
    }
    
    return (result + M) % M;
}

// Garner's Algorithm: Alternative CRT implementation
// More efficient when moduli are large
long long garnerAlgorithm(const std::vector<long long>& a, 
                         const std::vector<long long>& m) {
    int n = a.size();
    std::vector<long long> x(n);
    
    for (int i = 0; i < n; i++) {
        x[i] = a[i];
        for (int j = 0; j < i; j++) {
            long long inv = modInverse(m[j], m[i]);
            if (inv == -1) return -1;
            x[i] = (x[i] - x[j]) * inv % m[i];
            if (x[i] < 0) x[i] += m[i];
        }
    }
    
    long long result = 0;
    long long mult = 1;
    
    for (int i = 0; i < n; i++) {
        result += x[i] * mult;
        mult *= m[i];
    }
    
    return result;
}

// Example usage
int main() {
    std::vector<long long> a = {2, 3, 2};
    std::vector<long long> m = {3, 5, 7};
    
    long long result1 = chineseRemainderTheorem(a, m);
    long long result2 = garnerAlgorithm(a, m);
    
    std::cout << "CRT solution: x ≡ " << result1 << " (mod " 
              << (m[0] * m[1] * m[2]) << ")" << std::endl;
    std::cout << "Garner's algorithm: " << result2 << std::endl;
    
    std::cout << "\nVerification:" << std::endl;
    for (size_t i = 0; i < a.size(); i++) {
        std::cout << result1 << " mod " << m[i] << " = " 
                  << (result1 % m[i]) << " (expected " << a[i] << ")" << std::endl;
    }
    
    return 0;
}

