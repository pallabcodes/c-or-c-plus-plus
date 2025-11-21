#include <iostream>
#include <vector>
#include <immintrin.h>
#include <cstring>

using namespace std;

// SIMD optimized array operations using AVX2
// Processes 8 integers at a time for massive speedup

// Sum of array elements using SIMD
int arraySumSIMD(const vector<int>& arr) {
    size_t size = arr.size();
    size_t simdSize = size - (size % 8);
    
    __m256i sumVec = _mm256_setzero_si256();
    const int* data = arr.data();

    // Process 8 elements at a time
    for (size_t i = 0; i < simdSize; i += 8) {
        __m256i vec = _mm256_loadu_si256((__m256i*)(data + i));
        sumVec = _mm256_add_epi32(sumVec, vec);
    }

    // Horizontal sum
    int sum[8];
    _mm256_storeu_si256((__m256i*)sum, sumVec);
    int total = sum[0] + sum[1] + sum[2] + sum[3] + 
                sum[4] + sum[5] + sum[6] + sum[7];

    // Handle remainder
    for (size_t i = simdSize; i < size; i++) {
        total += arr[i];
    }

    return total;
}

// Find maximum using SIMD
int arrayMaxSIMD(const vector<int>& arr) {
    size_t size = arr.size();
    size_t simdSize = size - (size % 8);
    
    __m256i maxVec = _mm256_set1_epi32(INT_MIN);
    const int* data = arr.data();

    for (size_t i = 0; i < simdSize; i += 8) {
        __m256i vec = _mm256_loadu_si256((__m256i*)(data + i));
        maxVec = _mm256_max_epi32(maxVec, vec);
    }

    int maxVals[8];
    _mm256_storeu_si256((__m256i*)maxVals, maxVec);
    int maxVal = maxVals[0];
    for (int i = 1; i < 8; i++) {
        if (maxVals[i] > maxVal) {
            maxVal = maxVals[i];
        }
    }

    for (size_t i = simdSize; i < size; i++) {
        if (arr[i] > maxVal) {
            maxVal = arr[i];
        }
    }

    return maxVal;
}

// Element-wise addition: result = a + b
vector<int> arrayAddSIMD(const vector<int>& a, const vector<int>& b) {
    size_t size = min(a.size(), b.size());
    size_t simdSize = size - (size % 8);
    
    vector<int> result(size);
    const int* aData = a.data();
    const int* bData = b.data();
    int* resultData = result.data();

    for (size_t i = 0; i < simdSize; i += 8) {
        __m256i vecA = _mm256_loadu_si256((__m256i*)(aData + i));
        __m256i vecB = _mm256_loadu_si256((__m256i*)(bData + i));
        __m256i vecSum = _mm256_add_epi32(vecA, vecB);
        _mm256_storeu_si256((__m256i*)(resultData + i), vecSum);
    }

    for (size_t i = simdSize; i < size; i++) {
        result[i] = a[i] + b[i];
    }

    return result;
}

// Dot product using SIMD
int dotProductSIMD(const vector<int>& a, const vector<int>& b) {
    size_t size = min(a.size(), b.size());
    size_t simdSize = size - (size % 8);
    
    __m256i sumVec = _mm256_setzero_si256();
    const int* aData = a.data();
    const int* bData = b.data();

    for (size_t i = 0; i < simdSize; i += 8) {
        __m256i vecA = _mm256_loadu_si256((__m256i*)(aData + i));
        __m256i vecB = _mm256_loadu_si256((__m256i*)(bData + i));
        __m256i vecMul = _mm256_mullo_epi32(vecA, vecB);
        sumVec = _mm256_add_epi32(sumVec, vecMul);
    }

    int sum[8];
    _mm256_storeu_si256((__m256i*)sum, sumVec);
    int total = sum[0] + sum[1] + sum[2] + sum[3] + 
                sum[4] + sum[5] + sum[6] + sum[7];

    for (size_t i = simdSize; i < size; i++) {
        total += a[i] * b[i];
    }

    return total;
}

// Count elements equal to value using SIMD
int countEqualSIMD(const vector<int>& arr, int value) {
    size_t size = arr.size();
    size_t simdSize = size - (size % 8);
    
    __m256i valueVec = _mm256_set1_epi32(value);
    __m256i countVec = _mm256_setzero_si256();
    const int* data = arr.data();

    for (size_t i = 0; i < simdSize; i += 8) {
        __m256i vec = _mm256_loadu_si256((__m256i*)(data + i));
        __m256i cmp = _mm256_cmpeq_epi32(vec, valueVec);
        countVec = _mm256_sub_epi32(countVec, cmp);
    }

    int counts[8];
    _mm256_storeu_si256((__m256i*)counts, countVec);
    int total = counts[0] + counts[1] + counts[2] + counts[3] + 
                counts[4] + counts[5] + counts[6] + counts[7];

    for (size_t i = simdSize; i < size; i++) {
        if (arr[i] == value) {
            total++;
        }
    }

    return total;
}

int main() {
    vector<int> arr1(1000, 1);
    vector<int> arr2(1000, 2);

    cout << "Array sum (SIMD): " << arraySumSIMD(arr1) << endl;
    cout << "Array max (SIMD): " << arrayMaxSIMD(arr1) << endl;
    
    vector<int> sum = arrayAddSIMD(arr1, arr2);
    cout << "Array add result[0]: " << sum[0] << endl;
    
    cout << "Dot product: " << dotProductSIMD(arr1, arr2) << endl;
    cout << "Count equal to 1: " << countEqualSIMD(arr1, 1) << endl;

    return 0;
}

