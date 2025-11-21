// Fast Fourier Transform (FFT): Efficient polynomial multiplication
// Converts between coefficient and point-value representations
// Time: O(n log n) for polynomial multiplication
// Space: O(n)

#include <vector>
#include <complex>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <iomanip>

using namespace std;
using cd = complex<double>;
const double PI = acos(-1);

// FFT: Cooley-Tukey algorithm
void fft(vector<cd>& a, bool invert) {
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
    
    // FFT
    for (int len = 2; len <= n; len <<= 1) {
        double ang = 2 * PI / len * (invert ? -1 : 1);
        cd wlen(cos(ang), sin(ang));
        
        for (int i = 0; i < n; i += len) {
            cd w(1);
            for (int j = 0; j < len / 2; j++) {
                cd u = a[i + j];
                cd v = a[i + j + len / 2] * w;
                a[i + j] = u + v;
                a[i + j + len / 2] = u - v;
                w *= wlen;
            }
        }
    }
    
    if (invert) {
        for (cd& x : a) {
            x /= n;
        }
    }
}

// Multiply two polynomials using FFT
vector<long long> multiplyPolynomials(const vector<long long>& a, 
                                     const vector<long long>& b) {
    vector<cd> fa(a.begin(), a.end());
    vector<cd> fb(b.begin(), b.end());
    
    int n = 1;
    while (n < a.size() + b.size()) {
        n <<= 1;
    }
    
    fa.resize(n);
    fb.resize(n);
    
    fft(fa, false);
    fft(fb, false);
    
    for (int i = 0; i < n; i++) {
        fa[i] *= fb[i];
    }
    
    fft(fa, true);
    
    vector<long long> result(n);
    for (int i = 0; i < n; i++) {
        result[i] = round(fa[i].real());
    }
    
    return result;
}

// Example usage
int main() {
    // Multiply (1 + 2x + 3x^2) * (4 + 5x)
    vector<long long> poly1 = {1, 2, 3};
    vector<long long> poly2 = {4, 5};
    
    vector<long long> result = multiplyPolynomials(poly1, poly2);
    
    cout << "Polynomial multiplication result: ";
    for (size_t i = 0; i < result.size(); i++) {
        if (result[i] != 0) {
            cout << result[i];
            if (i > 0) cout << "x^" << i;
            if (i < result.size() - 1 && result[i + 1] != 0) cout << " + ";
        }
    }
    cout << endl;
    
    return 0;
}

