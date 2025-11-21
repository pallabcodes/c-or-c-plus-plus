// Number Theoretic Transform (NTT): FFT over finite field
// Uses modular arithmetic instead of complex numbers
// Faster and exact for integer polynomials
// Time: O(n log n)
// Space: O(n)

#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

const int MOD = 998244353; // Common NTT modulus
const int ROOT = 3; // Primitive root modulo MOD

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

// NTT: Number Theoretic Transform
void ntt(vector<long long>& a, bool invert) {
    int n = a.size();
    
    // Bit-reversal permutation
    for (int i = 1, j = 0; i < n; i++) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) {
            j ^= bit;
        }
        j ^= bit;
        if (i < j) {
            swap(a[i], a[j]);
        }
    }
    
    // NTT
    for (int len = 2; len <= n; len <<= 1) {
        long long wlen = modPow(ROOT, (MOD - 1) / len, MOD);
        if (invert) {
            wlen = modInverse(wlen, MOD);
        }
        
        for (int i = 0; i < n; i += len) {
            long long w = 1;
            for (int j = 0; j < len / 2; j++) {
                long long u = a[i + j];
                long long v = (a[i + j + len / 2] * w) % MOD;
                a[i + j] = (u + v) % MOD;
                a[i + j + len / 2] = (u - v + MOD) % MOD;
                w = (w * wlen) % MOD;
            }
        }
    }
    
    if (invert) {
        long long invN = modInverse(n, MOD);
        for (long long& x : a) {
            x = (x * invN) % MOD;
        }
    }
}

// Multiply two polynomials using NTT
vector<long long> multiplyPolynomialsNTT(const vector<long long>& a,
                                         const vector<long long>& b) {
    vector<long long> fa(a.begin(), a.end());
    vector<long long> fb(b.begin(), b.end());
    
    int n = 1;
    while (n < a.size() + b.size()) {
        n <<= 1;
    }
    
    fa.resize(n);
    fb.resize(n);
    
    ntt(fa, false);
    ntt(fb, false);
    
    for (int i = 0; i < n; i++) {
        fa[i] = (fa[i] * fb[i]) % MOD;
    }
    
    ntt(fa, true);
    
    return fa;
}

// Example usage
int main() {
    vector<long long> poly1 = {1, 2, 3};
    vector<long long> poly2 = {4, 5};
    
    vector<long long> result = multiplyPolynomialsNTT(poly1, poly2);
    
    cout << "NTT polynomial multiplication result: ";
    for (size_t i = 0; i < result.size() && i < 5; i++) {
        cout << result[i] << " ";
    }
    cout << endl;
    
    return 0;
}

