// Pollard's Rho Algorithm: Fast factorization algorithm
// Finds a non-trivial factor of a composite number
// Time: O(sqrt(p)) where p is smallest prime factor
// Space: O(1)

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

using namespace std;

long long gcd(long long a, long long b) {
    while (b) {
        long long temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

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

// Pollard's Rho algorithm
long long pollardRho(long long n) {
    if (n % 2 == 0) return 2;
    if (n % 3 == 0) return 3;
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<long long> dis(2, n - 1);
    
    long long x = dis(gen);
    long long y = x;
    long long c = dis(gen);
    long long d = 1;
    
    // Floyd's cycle detection
    while (d == 1) {
        x = (modPow(x, 2, n) + c) % n;
        y = (modPow(y, 2, n) + c) % n;
        y = (modPow(y, 2, n) + c) % n;
        d = gcd(abs(x - y), n);
        
        if (d == n) {
            // Cycle detected, try again
            return pollardRho(n);
        }
    }
    
    return d;
}

// Complete factorization using Pollard's Rho
vector<long long> factorize(long long n) {
    vector<long long> factors;
    
    if (n == 1) return factors;
    if (n == 2 || n == 3) {
        factors.push_back(n);
        return factors;
    }
    
    // Remove small factors
    while (n % 2 == 0) {
        factors.push_back(2);
        n /= 2;
    }
    
    while (n % 3 == 0) {
        factors.push_back(3);
        n /= 3;
    }
    
    if (n == 1) return factors;
    
    // Use Pollard's Rho for remaining factors
    vector<long long> stack = {n};
    
    while (!stack.empty()) {
        long long num = stack.back();
        stack.pop_back();
        
        if (num == 1) continue;
        
        // Check if prime (simple check)
        bool isPrime = true;
        for (long long i = 2; i * i <= num && i < 1000; i++) {
            if (num % i == 0) {
                isPrime = false;
                break;
            }
        }
        
        if (isPrime) {
            factors.push_back(num);
        } else {
            long long factor = pollardRho(num);
            stack.push_back(factor);
            stack.push_back(num / factor);
        }
    }
    
    sort(factors.begin(), factors.end());
    return factors;
}

// Example usage
int main() {
    vector<long long> testNumbers = {
        60, 100, 123456789, 987654321, 2147483647
    };
    
    cout << "Pollard's Rho Factorization:" << endl;
    for (long long n : testNumbers) {
        cout << n << " = ";
        vector<long long> factors = factorize(n);
        for (size_t i = 0; i < factors.size(); i++) {
            cout << factors[i];
            if (i < factors.size() - 1) cout << " * ";
        }
        cout << endl;
    }
    
    return 0;
}

