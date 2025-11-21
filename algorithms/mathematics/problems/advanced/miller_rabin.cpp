// Miller-Rabin Primality Test: Probabilistic test for prime numbers
// Very fast and accurate for large numbers
// Time: O(k log^3 n) where k is number of rounds
// Space: O(1)

#include <iostream>
#include <random>
#include <vector>

using namespace std;

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

// Miller-Rabin test
bool millerRabin(long long n, int rounds = 10) {
    if (n < 2) return false;
    if (n == 2 || n == 3) return true;
    if (n % 2 == 0) return false;
    
    // Write n-1 as d * 2^r
    long long d = n - 1;
    int r = 0;
    while (d % 2 == 0) {
        d /= 2;
        r++;
    }
    
    // Witness bases for deterministic test up to certain limits
    vector<long long> bases;
    if (n < 2047) {
        bases = {2};
    } else if (n < 1373653) {
        bases = {2, 3};
    } else if (n < 9080191) {
        bases = {31, 73};
    } else if (n < 25326001) {
        bases = {2, 3, 5};
    } else if (n < 3215031751LL) {
        bases = {2, 3, 5, 7};
    } else {
        // For larger numbers, use random bases
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<long long> dis(2, n - 2);
        for (int i = 0; i < rounds; i++) {
            bases.push_back(dis(gen));
        }
    }
    
    for (long long a : bases) {
        long long x = modPow(a, d, n);
        
        if (x == 1 || x == n - 1) {
            continue;
        }
        
        bool composite = true;
        for (int i = 0; i < r - 1; i++) {
            x = (x * x) % n;
            if (x == n - 1) {
                composite = false;
                break;
            }
        }
        
        if (composite) {
            return false; // Definitely composite
        }
    }
    
    return true; // Probably prime
}

// Example usage
int main() {
    vector<long long> testNumbers = {
        2, 3, 5, 7, 11, 13, 17, 19, 23, 29,
        31, 37, 41, 43, 47, 53, 59, 61, 67, 71,
        4, 6, 8, 9, 10, 12, 14, 15, 16, 18,
        1000000007, 2147483647, 982451653
    };
    
    cout << "Miller-Rabin Primality Test:" << endl;
    for (long long n : testNumbers) {
        bool isPrime = millerRabin(n);
        cout << n << " is " << (isPrime ? "prime" : "composite") << endl;
    }
    
    return 0;
}

