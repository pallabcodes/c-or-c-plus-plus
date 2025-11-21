// Lucas' Theorem: Compute binomial coefficients modulo prime
// Based on research by Édouard Lucas
// Time: O(p^2 * log_p(n))
// Space: O(p)
// God modded implementation for large binomial coefficients

#include <vector>
#include <iostream>
#include <algorithm>

long long modPow(long long base, long long exp, long long mod) {
    long long result = 1;
    base %= mod;
    
    while (exp > 0) {
        if (exp & 1) {
            result = (result * base) % mod;
        }
        base = (base * base) % mod;
        exp >>= 1;
    }
    
    return result;
}

long long modInverse(long long a, long long mod) {
    return modPow(a, mod - 2, mod);
}

// Precompute factorials and inverse factorials modulo p
void precomputeFactorials(int p, std::vector<long long>& fact, 
                         std::vector<long long>& invFact) {
    fact[0] = 1;
    invFact[0] = 1;
    
    for (int i = 1; i < p; i++) {
        fact[i] = (fact[i - 1] * i) % p;
        invFact[i] = modInverse(fact[i], p);
    }
}

// Binomial coefficient C(n, k) mod p using Lucas theorem
long long binomialModP(long long n, long long k, int p, 
                      const std::vector<long long>& fact,
                      const std::vector<long long>& invFact) {
    if (k < 0 || k > n) return 0;
    if (k == 0 || k == n) return 1;
    
    long long result = 1;
    
    while (n > 0 || k > 0) {
        int ni = n % p;
        int ki = k % p;
        
        if (ki > ni) return 0;
        
        result = (result * fact[ni]) % p;
        result = (result * invFact[ki]) % p;
        result = (result * invFact[ni - ki]) % p;
        
        n /= p;
        k /= p;
    }
    
    return result;
}

// Lucas theorem implementation
long long lucasTheorem(long long n, long long k, int p) {
    std::vector<long long> fact(p), invFact(p);
    precomputeFactorials(p, fact, invFact);
    
    return binomialModP(n, k, p, fact, invFact);
}

// Example usage
int main() {
    long long n = 1000;
    long long k = 500;
    int p = 1009;
    
    long long result = lucasTheorem(n, k, p);
    
    std::cout << "C(" << n << ", " << k << ") mod " << p << " = " << result << std::endl;
    
    n = 10;
    k = 3;
    p = 7;
    result = lucasTheorem(n, k, p);
    std::cout << "C(" << n << ", " << k << ") mod " << p << " = " << result << std::endl;
    
    return 0;
}

