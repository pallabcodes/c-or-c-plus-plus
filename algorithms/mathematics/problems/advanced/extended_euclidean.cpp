// Extended Euclidean Algorithm: Find GCD and coefficients
// Solves ax + by = gcd(a, b)
// Also finds modular inverse
// Time: O(log min(a, b))
// Space: O(1)

#include <iostream>
#include <tuple>

using namespace std;

// Extended Euclidean Algorithm
// Returns (gcd(a, b), x, y) such that ax + by = gcd(a, b)
tuple<long long, long long, long long> extendedGCD(long long a, long long b) {
    if (b == 0) {
        return make_tuple(a, 1, 0);
    }
    
    auto [gcd, x1, y1] = extendedGCD(b, a % b);
    long long x = y1;
    long long y = x1 - (a / b) * y1;
    
    return make_tuple(gcd, x, y);
}

// Find modular inverse of a modulo m
// Returns x such that (a * x) % m = 1
long long modInverse(long long a, long long m) {
    auto [gcd, x, y] = extendedGCD(a, m);
    
    if (gcd != 1) {
        // Modular inverse doesn't exist
        return -1;
    }
    
    return (x % m + m) % m;
}

// Solve linear Diophantine equation: ax + by = c
// Returns (x0, y0) - particular solution, or (-1, -1) if no solution
pair<long long, long long> solveDiophantine(long long a, long long b, long long c) {
    auto [g, x0, y0] = extendedGCD(abs(a), abs(b));
    
    if (c % g != 0) {
        return make_pair(-1, -1); // No solution
    }
    
    x0 *= c / g;
    y0 *= c / g;
    
    if (a < 0) x0 = -x0;
    if (b < 0) y0 = -y0;
    
    return make_pair(x0, y0);
}

// Chinese Remainder Theorem
// Solve system: x ≡ a1 (mod m1), x ≡ a2 (mod m2), ...
long long chineseRemainderTheorem(const vector<long long>& a, 
                                   const vector<long long>& m) {
    int n = a.size();
    long long M = 1;
    
    for (long long mi : m) {
        M *= mi;
    }
    
    long long result = 0;
    
    for (int i = 0; i < n; i++) {
        long long Mi = M / m[i];
        long long inv = modInverse(Mi, m[i]);
        result = (result + a[i] * Mi * inv) % M;
    }
    
    return result;
}

// Example usage
int main() {
    // Extended GCD example
    long long a = 35, b = 15;
    auto [g, x, y] = extendedGCD(a, b);
    
    cout << "Extended GCD of " << a << " and " << b << ":" << endl;
    cout << "GCD = " << g << endl;
    cout << "Coefficients: " << a << " * " << x << " + " 
         << b << " * " << y << " = " << g << endl;
    
    // Modular inverse example
    long long num = 7, mod = 11;
    long long inv = modInverse(num, mod);
    cout << "\nModular inverse of " << num << " mod " << mod 
         << " = " << inv << endl;
    
    // Diophantine equation example
    auto [x0, y0] = solveDiophantine(35, 15, 10);
    cout << "\nSolution to 35x + 15y = 10: x = " << x0 << ", y = " << y0 << endl;
    
    // Chinese Remainder Theorem example
    vector<long long> a_values = {2, 3, 2};
    vector<long long> m_values = {3, 5, 7};
    long long crt_result = chineseRemainderTheorem(a_values, m_values);
    cout << "\nChinese Remainder Theorem:" << endl;
    cout << "x ≡ 2 (mod 3), x ≡ 3 (mod 5), x ≡ 2 (mod 7)" << endl;
    cout << "Solution: x = " << crt_result << endl;
    
    return 0;
}

