/*
 * Levinson Recursion for Toeplitz Matrices
 * 
 * Source: "The Wiener RMS (Root Mean Square) Error Criterion in Filter Design 
 *          and Prediction" by Norman Levinson (1947)
 * Paper: Journal of Mathematics and Physics, 25(1-4), 261-278
 * Algorithm: Recursive solution to Toeplitz systems of linear equations
 * 
 * What Makes It Ingenious:
 * - Recursive solution: O(n^2) instead of O(n^3) for Gaussian elimination
 * - Toeplitz structure: Constant along diagonals, exploited recursively
 * - Levinson-Durbin: Special case for symmetric positive definite Toeplitz
 * - Used in signal processing, time series analysis, AR modeling
 * - Recursive computation of solution vectors
 * 
 * When to Use:
 * - Toeplitz matrix systems (constant along diagonals)
 * - Autoregressive (AR) model estimation
 * - Linear prediction coefficients
 * - Signal processing applications
 * - Time series analysis
 * - Yule-Walker equations
 * 
 * Real-World Usage:
 * - Speech coding (LPC - Linear Predictive Coding)
 * - Audio compression
 * - Time series forecasting
 * - Signal filtering
 * - Autocorrelation analysis
 * 
 * Time Complexity: O(n^2) instead of O(n^3) for general systems
 * Space Complexity: O(n) for storing vectors
 */

#include <vector>
#include <cmath>
#include <stdexcept>
#include <iostream>

class LevinsonRecursion {
public:
    // Solve Toeplitz system: T * x = b
    // where T is Toeplitz matrix with first row [t0, t1, t2, ..., t_{n-1}]
    // and first column [t0, t_{-1}, t_{-2}, ..., t_{-(n-1)}]
    static std::vector<double> solve_toeplitz(
        const std::vector<double>& first_row,
        const std::vector<double>& first_col,
        const std::vector<double>& rhs) {
        
        int n = rhs.size();
        
        if (first_row.size() != n || first_col.size() != n) {
            throw std::invalid_argument("Matrix dimensions must match");
        }
        
        if (first_row[0] != first_col[0]) {
            throw std::invalid_argument("Diagonal elements must match");
        }
        
        // Initialize solution for size 1
        std::vector<double> x(n, 0.0);
        x[0] = rhs[0] / first_row[0];
        
        // Forward vector (solution for current size)
        std::vector<double> forward(n, 0.0);
        forward[0] = 1.0;
        
        // Backward vector (reversed solution)
        std::vector<double> backward(n, 0.0);
        backward[0] = 1.0;
        
        // Recursively build solution for sizes 1, 2, ..., n
        for (int m = 1; m < n; m++) {
            // Compute reflection coefficient (alpha)
            double alpha = 0.0;
            for (int i = 0; i < m; i++) {
                alpha += first_row[m - i] * backward[i];
            }
            
            // Compute denominator (beta)
            double beta = 0.0;
            for (int i = 0; i < m; i++) {
                beta += first_col[m - i] * forward[i];
            }
            
            double gamma = first_row[0] - beta;
            
            if (std::abs(gamma) < 1e-10) {
                throw std::runtime_error("Matrix is singular or ill-conditioned");
            }
            
            // Compute error
            double error = 0.0;
            for (int i = 0; i < m; i++) {
                error += first_row[m - i] * x[i];
            }
            error = rhs[m] - error;
            
            // Update solution
            for (int i = 0; i < m; i++) {
                x[i] += (error / gamma) * backward[m - 1 - i];
            }
            x[m] = error / gamma;
            
            // Update forward and backward vectors for next iteration
            std::vector<double> new_forward(m + 1, 0.0);
            std::vector<double> new_backward(m + 1, 0.0);
            
            for (int i = 0; i < m; i++) {
                new_forward[i] = forward[i] - (alpha / gamma) * backward[m - 1 - i];
                new_backward[i] = backward[i] - (beta / gamma) * forward[m - 1 - i];
            }
            new_forward[m] = -alpha / gamma;
            new_backward[m] = -beta / gamma;
            
            forward = std::move(new_forward);
            backward = std::move(new_backward);
        }
        
        return x;
    }
    
    // Levinson-Durbin recursion for symmetric positive definite Toeplitz
    // Solves: T * a = -r where T is symmetric Toeplitz
    // Returns: solution vector a and prediction error variance
    static std::pair<std::vector<double>, double> levinson_durbin(
        const std::vector<double>& autocorrelation) {
        
        int n = autocorrelation.size() - 1;  // Order of AR model
        
        if (n <= 0) {
            throw std::invalid_argument("Invalid autocorrelation vector");
        }
        
        // Initialize
        std::vector<double> a(n, 0.0);
        double error_variance = autocorrelation[0];
        
        // Recursively compute AR coefficients
        for (int m = 1; m <= n; m++) {
            // Compute reflection coefficient (PARCOR coefficient)
            double k = autocorrelation[m];
            for (int i = 1; i < m; i++) {
                k += a[i - 1] * autocorrelation[m - i];
            }
            k = -k / error_variance;
            
            // Update coefficients
            std::vector<double> new_a(m);
            for (int i = 0; i < m - 1; i++) {
                new_a[i] = a[i] + k * a[m - 2 - i];
            }
            new_a[m - 1] = k;
            
            // Update error variance
            error_variance *= (1.0 - k * k);
            
            a = std::move(new_a);
        }
        
        return {a, error_variance};
    }
    
    // Compute linear prediction coefficients using Levinson-Durbin
    static std::vector<double> linear_prediction_coefficients(
        const std::vector<double>& signal,
        int order) {
        
        int n = signal.size();
        if (order >= n) {
            throw std::invalid_argument("Order must be less than signal length");
        }
        
        // Compute autocorrelation
        std::vector<double> autocorrelation(order + 1, 0.0);
        
        for (int lag = 0; lag <= order; lag++) {
            for (int i = 0; i < n - lag; i++) {
                autocorrelation[lag] += signal[i] * signal[i + lag];
            }
            autocorrelation[lag] /= (n - lag);
        }
        
        // Normalize by autocorrelation[0]
        if (autocorrelation[0] > 1e-10) {
            for (int i = 1; i <= order; i++) {
                autocorrelation[i] /= autocorrelation[0];
            }
            autocorrelation[0] = 1.0;
        }
        
        // Apply Levinson-Durbin
        auto [coefficients, error_var] = levinson_durbin(autocorrelation);
        
        return coefficients;
    }
};

// Example usage
int main() {
    // Example 1: Solve Toeplitz system
    std::vector<double> first_row = {2.0, 1.0, 0.5};
    std::vector<double> first_col = {2.0, 1.0, 0.5};
    std::vector<double> rhs = {1.0, 0.0, 0.0};
    
    try {
        auto solution = LevinsonRecursion::solve_toeplitz(
            first_row, first_col, rhs);
        
        std::cout << "Toeplitz system solution:" << std::endl;
        for (size_t i = 0; i < solution.size(); i++) {
            std::cout << "x[" << i << "] = " << solution[i] << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    // Example 2: Linear prediction coefficients
    std::vector<double> signal = {1.0, 2.0, 3.0, 2.0, 1.0, 2.0, 3.0, 2.0};
    int order = 3;
    
    try {
        auto lpc = LevinsonRecursion::linear_prediction_coefficients(signal, order);
        
        std::cout << "\nLinear Prediction Coefficients (order " << order << "):" << std::endl;
        for (size_t i = 0; i < lpc.size(); i++) {
            std::cout << "a[" << i << "] = " << lpc[i] << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    return 0;
}

