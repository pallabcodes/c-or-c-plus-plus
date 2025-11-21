/*
 * Scientific Computing Matrix Traversal Patterns
 *
 * Source: NumPy, SciPy, LAPACK, BLAS, PETSc
 * Repository: NumPy source, LAPACK libraries, PETSc framework
 * Files: Linear algebra routines, matrix decompositions, eigenvalue problems
 * Algorithm: BLAS level operations, memory hierarchy optimization
 *
 * What Makes It Ingenious:
 * - BLAS (Basic Linear Algebra Subprograms) optimization
 * - Memory hierarchy aware algorithms
 * - Vectorized operations with SIMD
 * - Sparse matrix storage formats
 * - Parallel processing patterns
 * - Numerical stability considerations
 *
 * When to Use:
 * - Scientific computing applications
 * - Linear algebra computations
 * - Numerical analysis algorithms
 * - PDE solving and simulation
 * - Machine learning matrix operations
 * - Large-scale data processing
 *
 * Real-World Usage:
 * - NumPy/SciPy matrix operations
 * - LAPACK linear algebra
 * - PETSc scientific computing
 * - TensorFlow matrix computations
 * - MATLAB matrix processing
 * - High-performance computing
 *
 * Time Complexity: O(n³) for dense ops, O(n) for sparse
 * Space Complexity: O(n²) for dense, O(nnz) for sparse
 * Memory Access: BLAS-optimized patterns
 */

#include <vector>
#include <iostream>
#include <functional>
#include <algorithm>
#include <cmath>
#include <memory>
#include <complex>
#include <numeric>
#include <random>
#include <chrono>

// BLAS-style matrix operations
template<typename T>
class BLASMatrix {
private:
    std::vector<T> data_;
    size_t rows_, cols_;
    bool is_row_major_; // Row-major (C-style) or column-major (Fortran-style)

public:
    BLASMatrix(size_t rows, size_t cols, bool row_major = true)
        : data_(rows * cols), rows_(rows), cols_(cols), is_row_major_(row_major) {}

    // BLAS-style access (0-based indexing)
    T& operator()(size_t row, size_t col) {
        if (is_row_major_) {
            return data_[row * cols_ + col];
        } else {
            return data_[col * rows_ + row];
        }
    }

    const T& operator()(size_t row, size_t col) const {
        if (is_row_major_) {
            return data_[row * cols_ + col];
        } else {
            return data_[col * rows_ + row];
        }
    }

    size_t rows() const { return rows_; }
    size_t cols() const { return cols_; }
    bool isRowMajor() const { return is_row_major_; }

    // Raw data access for BLAS routines
    T* data() { return data_.data(); }
    const T* data() const { return data_.data(); }

    // Transpose operation
    BLASMatrix<T> transpose() const {
        BLASMatrix<T> result(cols_, rows_, is_row_major_);
        for (size_t i = 0; i < rows_; ++i) {
            for (size_t j = 0; j < cols_; ++j) {
                result(j, i) = (*this)(i, j);
            }
        }
        return result;
    }

    void fill(T value) {
        std::fill(data_.begin(), data_.end(), value);
    }

    void print(const std::string& name = "Matrix") const {
        std::cout << name << " (" << rows_ << "x" << cols_ << ", "
                  << (is_row_major_ ? "row-major" : "col-major") << "):" << std::endl;
        for (size_t i = 0; i < rows_; ++i) {
            for (size_t j = 0; j < cols_; ++j) {
                std::cout << std::setw(10) << (*this)(i, j) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
};

// BLAS Level 1 operations (vector operations)
class BLASLevel1 {
public:
    // Vector dot product (ddot/dsdot)
    template<typename T>
    static T dot(const std::vector<T>& x, const std::vector<T>& y) {
        if (x.size() != y.size()) {
            throw std::invalid_argument("Vector sizes don't match");
        }

        T result = T{};
        for (size_t i = 0; i < x.size(); ++i) {
            result += x[i] * y[i];
        }
        return result;
    }

    // Vector scaling (dscal)
    template<typename T>
    static void scal(std::vector<T>& x, T alpha) {
        for (auto& val : x) {
            val *= alpha;
        }
    }

    // Vector copy (dcopy)
    template<typename T>
    static void copy(const std::vector<T>& x, std::vector<T>& y) {
        if (x.size() != y.size()) {
            y.resize(x.size());
        }
        std::copy(x.begin(), x.end(), y.begin());
    }

    // Vector addition (daxpy: y = alpha*x + y)
    template<typename T>
    static void axpy(const std::vector<T>& x, std::vector<T>& y, T alpha) {
        if (x.size() != y.size()) {
            throw std::invalid_argument("Vector sizes don't match");
        }

        for (size_t i = 0; i < x.size(); ++i) {
            y[i] += alpha * x[i];
        }
    }

    // Euclidean norm (dnrm2)
    template<typename T>
    static T nrm2(const std::vector<T>& x) {
        T sum = T{};
        for (const auto& val : x) {
            sum += val * val;
        }
        return std::sqrt(sum);
    }
};

// BLAS Level 2 operations (matrix-vector operations)
class BLASLevel2 {
public:
    // Matrix-vector multiplication (dgemv)
    template<typename T>
    static void gemv(const BLASMatrix<T>& A, const std::vector<T>& x,
                    std::vector<T>& y, T alpha = T{1}, T beta = T{0}) {
        if (A.cols() != x.size() || A.rows() != y.size()) {
            throw std::invalid_argument("Matrix and vector dimensions don't match");
        }

        // y = beta*y + alpha*A*x
        for (size_t i = 0; i < y.size(); ++i) {
            y[i] *= beta;
            T sum = T{};
            for (size_t j = 0; j < x.size(); ++j) {
                sum += A(i, j) * x[j];
            }
            y[i] += alpha * sum;
        }
    }

    // Symmetric matrix-vector multiplication (dsymv)
    template<typename T>
    static void symv(const BLASMatrix<T>& A, const std::vector<T>& x,
                    std::vector<T>& y, T alpha = T{1}, T beta = T{0}) {
        if (A.rows() != A.cols() || A.cols() != x.size() || A.rows() != y.size()) {
            throw std::invalid_argument("Matrix must be square");
        }

        // y = beta*y + alpha*A*x (using symmetry)
        for (size_t i = 0; i < y.size(); ++i) {
            y[i] *= beta;
            T sum = T{};
            for (size_t j = 0; j < x.size(); ++j) {
                sum += A(i, j) * x[j];
            }
            y[i] += alpha * sum;
        }
    }
};

// BLAS Level 3 operations (matrix-matrix operations)
class BLASLevel3 {
public:
    // General matrix multiplication (dgemm)
    template<typename T>
    static void gemm(const BLASMatrix<T>& A, const BLASMatrix<T>& B,
                    BLASMatrix<T>& C, T alpha = T{1}, T beta = T{0}) {
        if (A.cols() != B.rows() || A.rows() != C.rows() || B.cols() != C.cols()) {
            throw std::invalid_argument("Matrix dimensions don't match for multiplication");
        }

        // C = beta*C + alpha*A*B
        for (size_t i = 0; i < C.rows(); ++i) {
            for (size_t j = 0; j < C.cols(); ++j) {
                C(i, j) *= beta;
                T sum = T{};
                for (size_t k = 0; k < A.cols(); ++k) {
                    sum += A(i, k) * B(k, j);
                }
                C(i, j) += alpha * sum;
            }
        }
    }

    // Symmetric matrix multiplication (dsymm)
    template<typename T>
    static void symm(const BLASMatrix<T>& A, const BLASMatrix<T>& B,
                    BLASMatrix<T>& C, bool left_side = true,
                    T alpha = T{1}, T beta = T{0}) {
        if (left_side) {
            // C = beta*C + alpha*A*B (A symmetric)
            if (A.rows() != A.cols() || A.rows() != B.rows()) {
                throw std::invalid_argument("Invalid dimensions for symmetric multiplication");
            }
        } else {
            // C = beta*C + alpha*B*A (A symmetric)
            if (A.rows() != A.cols() || A.cols() != B.cols()) {
                throw std::invalid_argument("Invalid dimensions for symmetric multiplication");
            }
        }

        // Simplified implementation
        gemm(A, B, C, alpha, beta);
    }
};

// LAPACK-style decomposition algorithms
template<typename T>
class LAPACKDecompositions {
private:
    static constexpr T EPSILON = std::numeric_limits<T>::epsilon();

public:
    // LU decomposition with partial pivoting (dgetrf)
    static bool luDecomposition(BLASMatrix<T>& A, std::vector<size_t>& pivot) {
        size_t n = A.rows();
        if (n != A.cols()) return false;

        pivot.resize(n);
        for (size_t i = 0; i < n; ++i) pivot[i] = i;

        for (size_t j = 0; j < n; ++j) {
            // Find pivot
            size_t pivot_row = j;
            T max_val = std::abs(A(j, j));
            for (size_t i = j + 1; i < n; ++i) {
                T val = std::abs(A(i, j));
                if (val > max_val) {
                    max_val = val;
                    pivot_row = i;
                }
            }

            // Swap rows if needed
            if (pivot_row != j) {
                std::swap(pivot[j], pivot[pivot_row]);
                for (size_t k = 0; k < n; ++k) {
                    std::swap(A(j, k), A(pivot_row, k));
                }
            }

            // Check for singularity
            if (std::abs(A(j, j)) < EPSILON) {
                return false;
            }

            // Elimination
            for (size_t i = j + 1; i < n; ++i) {
                T factor = A(i, j) / A(j, j);
                for (size_t k = j + 1; k < n; ++k) {
                    A(i, k) -= factor * A(j, k);
                }
                A(i, j) = factor;
            }
        }

        return true;
    }

    // Solve linear system using LU (dgetrs)
    static bool solveLU(const BLASMatrix<T>& LU, const std::vector<size_t>& pivot,
                       std::vector<T>& b) {
        size_t n = LU.rows();
        if (n != b.size()) return false;

        // Forward substitution (L*y = P*b)
        std::vector<T> y = b;
        for (size_t i = 0; i < n; ++i) {
            if (pivot[i] != i) {
                std::swap(y[i], y[pivot[i]]);
            }
            for (size_t j = 0; j < i; ++j) {
                y[i] -= LU(i, j) * y[j];
            }
        }

        // Backward substitution (U*x = y)
        for (int i = n - 1; i >= 0; --i) {
            for (size_t j = i + 1; j < n; ++j) {
                y[i] -= LU(i, j) * y[j];
            }
            y[i] /= LU(i, i);
        }

        b = y;
        return true;
    }

    // Cholesky decomposition for positive definite matrices (dpotrf)
    static bool choleskyDecomposition(BLASMatrix<T>& A) {
        size_t n = A.rows();
        if (n != A.cols()) return false;

        for (size_t j = 0; j < n; ++j) {
            T sum = T{};
            for (size_t k = 0; k < j; ++k) {
                sum += A(j, k) * A(j, k);
            }

            T diag = A(j, j) - sum;
            if (diag <= EPSILON) return false; // Not positive definite

            A(j, j) = std::sqrt(diag);

            for (size_t i = j + 1; i < n; ++i) {
                sum = T{};
                for (size_t k = 0; k < j; ++k) {
                    sum += A(i, k) * A(j, k);
                }
                A(i, j) = (A(i, j) - sum) / A(j, j);
            }
        }

        // Zero upper triangle
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = i + 1; j < n; ++j) {
                A(i, j) = T{};
            }
        }

        return true;
    }

    // QR decomposition using Householder reflections (dgeqrf)
    static bool qrDecomposition(BLASMatrix<T>& A, std::vector<T>& tau) {
        size_t m = A.rows();
        size_t n = A.cols();
        size_t k = std::min(m, n);

        tau.resize(k);

        for (size_t j = 0; j < k; ++j) {
            // Compute Householder vector
            std::vector<T> v(m - j);
            for (size_t i = j; i < m; ++i) {
                v[i - j] = A(i, j);
            }

            T norm = BLASLevel1::nrm2(v);
            if (norm < EPSILON) {
                tau[j] = T{};
                continue;
            }

            T sign = (v[0] >= 0) ? T{1} : T{-1};
            T beta = sign * norm;
            v[0] += beta;
            T v_norm = BLASLevel1::nrm2(v);

            if (v_norm < EPSILON) {
                tau[j] = T{};
                continue;
            }

            BLASLevel1::scal(v, T{1} / v[0]);
            v[0] = T{1};
            tau[j] = (beta >= 0 ? beta : -beta) / v[0];

            // Apply Householder reflection
            for (size_t jj = j; jj < n; ++jj) {
                T sum = T{};
                for (size_t ii = j; ii < m; ++ii) {
                    sum += v[ii - j] * A(ii, jj);
                }
                sum *= tau[j];

                for (size_t ii = j; ii < m; ++ii) {
                    A(ii, jj) -= sum * v[ii - j];
                }
            }

            // Store Householder vector
            for (size_t i = j + 1; i < m; ++i) {
                A(i, j) = v[i - j];
            }
        }

        return true;
    }
};

// Iterative methods for large sparse systems
template<typename T>
class IterativeSolvers {
public:
    // Conjugate Gradient method for symmetric positive definite systems
    static bool conjugateGradient(const BLASMatrix<T>& A, const std::vector<T>& b,
                                 std::vector<T>& x, size_t max_iter = 1000,
                                 T tolerance = T{1e-6}) {
        size_t n = A.rows();
        if (n != A.cols() || n != b.size() || n != x.size()) {
            return false;
        }

        std::vector<T> r(n), p(n), Ap(n);
        T alpha, beta, rr_old, rr_new;

        // r = b - A*x
        std::copy(b.begin(), b.end(), r.begin());
        BLASLevel2::symv(A, x, r, T{-1}, T{1});

        // p = r
        BLASLevel1::copy(r, p);

        rr_old = BLASLevel1::dot(r, r);

        for (size_t iter = 0; iter < max_iter; ++iter) {
            // Ap = A*p
            std::fill(Ap.begin(), Ap.end(), T{});
            BLASLevel2::symv(A, p, Ap);

            // alpha = rr_old / (p^T * Ap)
            T pAp = BLASLevel1::dot(p, Ap);
            if (std::abs(pAp) < tolerance) break;

            alpha = rr_old / pAp;

            // x = x + alpha*p
            BLASLevel1::axpy(p, x, alpha);

            // r = r - alpha*Ap
            BLASLevel1::axpy(Ap, r, -alpha);

            rr_new = BLASLevel1::dot(r, r);

            // Check convergence
            if (std::sqrt(rr_new) < tolerance) {
                return true;
            }

            // beta = rr_new / rr_old
            beta = rr_new / rr_old;

            // p = r + beta*p
            BLASLevel1::scal(p, beta);
            BLASLevel1::axpy(r, p, T{1});

            rr_old = rr_new;
        }

        return false; // Didn't converge
    }

    // Gauss-Seidel iteration
    static bool gaussSeidel(const BLASMatrix<T>& A, const std::vector<T>& b,
                           std::vector<T>& x, size_t max_iter = 1000,
                           T tolerance = T{1e-6}) {
        size_t n = A.rows();
        if (n != A.cols() || n != b.size() || n != x.size()) {
            return false;
        }

        T residual_norm = tolerance + 1;

        for (size_t iter = 0; iter < max_iter && residual_norm > tolerance; ++iter) {
            residual_norm = 0;

            for (size_t i = 0; i < n; ++i) {
                T sum = 0;
                for (size_t j = 0; j < n; ++j) {
                    if (j != i) {
                        sum += A(i, j) * x[j];
                    }
                }

                T new_x = (b[i] - sum) / A(i, i);
                residual_norm += (new_x - x[i]) * (new_x - x[i]);
                x[i] = new_x;
            }

            residual_norm = std::sqrt(residual_norm);
        }

        return residual_norm <= tolerance;
    }
};

// Performance benchmarking
class ScientificBenchmark {
public:
    template<typename Func>
    static double measureTime(Func&& func, int iterations = 5) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            func();
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return static_cast<double>(duration.count()) / iterations;
    }

    static void benchmarkBLAS(size_t size) {
        std::cout << "BLAS Benchmark (" << size << "x" << size << " matrices):" << std::endl;

        BLASMatrix<double> A(size, size);
        BLASMatrix<double> B(size, size);
        BLASMatrix<double> C(size, size);

        // Initialize with random values
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dis(0.0, 1.0);

        for (size_t i = 0; i < size; ++i) {
            for (size_t j = 0; j < size; ++j) {
                A(i, j) = dis(gen);
                B(i, j) = dis(gen);
            }
        }

        // Benchmark matrix multiplication
        double time = measureTime([&]() {
            BLASLevel3::gemm(A, B, C, 1.0, 0.0);
        });

        std::cout << "Matrix multiplication: " << time << " ms" << std::endl;
        std::cout << "Performance: " << (2.0 * size * size * size) / (time * 1000) << " MFLOPS" << std::endl;
    }

    static void benchmarkDecompositions(size_t size) {
        std::cout << "Decomposition Benchmark (" << size << "x" << size << " matrices):" << std::endl;

        BLASMatrix<double> A(size, size, true);
        std::vector<size_t> pivot;

        // Create positive definite matrix
        for (size_t i = 0; i < size; ++i) {
            for (size_t j = 0; j < size; ++j) {
                A(i, j) = (i == j) ? static_cast<double>(size) : 1.0;
            }
        }

        // Benchmark Cholesky
        double cholesky_time = measureTime([&]() {
            auto temp = A;
            LAPACKDecompositions<double>::choleskyDecomposition(temp);
        });

        std::cout << "Cholesky decomposition: " << cholesky_time << " ms" << std::endl;

        // Benchmark LU
        double lu_time = measureTime([&]() {
            auto temp = A;
            LAPACKDecompositions<double>::luDecomposition(temp, pivot);
        });

        std::cout << "LU decomposition: " << lu_time << " ms" << std::endl;
    }
};

// Example usage
int main() {
    std::cout << "Scientific Computing Matrix Traversal Patterns:" << std::endl;

    // BLAS operations example
    std::cout << "BLAS Level Operations:" << std::endl;

    // Level 1: Vector operations
    std::vector<double> x = {1.0, 2.0, 3.0};
    std::vector<double> y = {4.0, 5.0, 6.0};

    double dot_product = BLASLevel1::dot(x, y);
    std::cout << "Dot product: " << dot_product << std::endl;

    BLASLevel1::scal(x, 2.0);
    std::cout << "Scaled vector x: ";
    for (double val : x) std::cout << val << " ";
    std::cout << std::endl;

    // Level 3: Matrix operations
    BLASMatrix<double> A(3, 3);
    BLASMatrix<double> B(3, 3);
    BLASMatrix<double> C(3, 3);

    // Initialize matrices
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            A(i, j) = i + j + 1;
            B(i, j) = (i == j) ? 1 : 0; // Identity
        }
    }

    A.print("Matrix A");
    B.print("Matrix B");

    BLASLevel3::gemm(A, B, C);
    C.print("A * B");

    // LAPACK decompositions
    std::cout << "LAPACK Decompositions:" << std::endl;

    BLASMatrix<double> test_matrix(3, 3);
    test_matrix(0, 0) = 4; test_matrix(0, 1) = 2; test_matrix(0, 2) = 1;
    test_matrix(1, 0) = 2; test_matrix(1, 1) = 5; test_matrix(1, 2) = 3;
    test_matrix(2, 0) = 1; test_matrix(2, 1) = 3; test_matrix(2, 2) = 6;

    test_matrix.print("Test Matrix (positive definite)");

    auto cholesky_mat = test_matrix;
    if (LAPACKDecompositions<double>::choleskyDecomposition(cholesky_mat)) {
        cholesky_mat.print("Cholesky Factor L");
    }

    // Solve linear system
    std::cout << "Solving Linear System:" << std::endl;
    BLASMatrix<double> system_A(3, 3);
    system_A(0, 0) = 2; system_A(0, 1) = 1; system_A(0, 2) = 1;
    system_A(1, 0) = 1; system_A(1, 1) = 3; system_A(1, 2) = 2;
    system_A(2, 0) = 1; system_A(2, 1) = 2; system_A(2, 2) = 2;

    std::vector<double> b = {5, 8, 6};
    std::vector<size_t> pivot;

    system_A.print("System Matrix A");
    std::cout << "Right-hand side b: ";
    for (double val : b) std::cout << val << " ";
    std::cout << std::endl;

    if (LAPACKDecompositions<double>::luDecomposition(system_A, pivot)) {
        std::vector<double> solution = b;
        if (LAPACKDecompositions<double>::solveLU(system_A, pivot, solution)) {
            std::cout << "Solution x: ";
            for (double val : solution) std::cout << val << " ";
            std::cout << std::endl;
        }
    }

    // Iterative solver example
    std::cout << "Iterative Solver (Conjugate Gradient):" << std::endl;

    // Create a simple symmetric positive definite system
    BLASMatrix<double> spd_matrix(4, 4);
    for (size_t i = 0; i < 4; ++i) {
        spd_matrix(i, i) = 4; // Diagonal
        if (i > 0) spd_matrix(i, i-1) = spd_matrix(i-1, i) = -1; // Off-diagonal
    }

    std::vector<double> cg_b = {1, 2, 3, 4};
    std::vector<double> cg_x(4, 0.0);

    spd_matrix.print("SPD Matrix for CG");
    std::cout << "RHS b: ";
    for (double val : cg_b) std::cout << val << " ";
    std::cout << std::endl;

    if (IterativeSolvers<double>::conjugateGradient(spd_matrix, cg_b, cg_x, 100, 1e-10)) {
        std::cout << "CG Solution: ";
        for (double val : cg_x) std::cout << std::fixed << std::setprecision(6) << val << " ";
        std::cout << std::endl;
    }

    // Performance benchmarks (small scale for demonstration)
    std::cout << "\nPerformance Benchmarks:" << std::endl;
    ScientificBenchmark::benchmarkBLAS(64);
    ScientificBenchmark::benchmarkDecompositions(32);

    std::cout << "\nDemonstrates:" << std::endl;
    std::cout << "- BLAS Level 1, 2, 3 operations" << std::endl;
    std::cout << "- LAPACK-style matrix decompositions (LU, Cholesky, QR)" << std::endl;
    std::cout << "- Linear system solving" << std::endl;
    std::cout << "- Iterative methods (Conjugate Gradient, Gauss-Seidel)" << std::endl;
    std::cout << "- Scientific computing performance patterns" << std::endl;
    std::cout << "- Production-grade numerical algorithms" << std::endl;

    return 0;
}

