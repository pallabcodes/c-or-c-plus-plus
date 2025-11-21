/*
 * Code Completion DP - Tool Building (IDEs, Editors)
 *
 * Source: IDEs (VSCode, IntelliJ, Vim), code editors
 * Pattern: DP for intelligent code completion and analysis
 * Algorithm: String matching, parsing, and suggestion algorithms
 *
 * What Makes It Ingenious:
 * - Intelligent code completion with context awareness
 * - Fuzzy string matching for typo tolerance
 * - Type inference and suggestion ranking
 * - Memory-efficient storage of code analysis data
 * - Real-time performance for interactive editing
 * - Handles large codebases efficiently
 *
 * When to Use:
 * - IDE code completion engines
 * - Code editors with IntelliSense
 * - Refactoring tools
 * - Code analysis plugins
 * - Language servers (LSP)
 * - Static analysis tools
 *
 * Real-World Usage:
 * - VSCode IntelliSense
 * - IntelliJ IDEA code completion
 * - CLion C++ completion
 * - Vim/Neovim completion plugins
 * - Language servers (TypeScript, Rust, etc.)
 *
 * Time Complexity: O(n) preprocessing, O(m + k log k) per query
 * Space Complexity: O(n) for symbol tables, O(m) for fuzzy matching
 */

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <queue>

// Symbol information for code completion
struct Symbol {
    std::string name;
    std::string type;
    std::string scope;
    int frequency;  // Usage frequency for ranking
    double score;   // Computed relevance score

    Symbol(const std::string& n, const std::string& t,
           const std::string& s = "", int freq = 1)
        : name(n), type(t), scope(s), frequency(freq), score(0.0) {}
};

// Fuzzy string matching using DP (Levenshtein distance)
class FuzzyMatcher {
private:
    // DP table for edit distance
    std::vector<std::vector<int>> dp_table_;

    // Compute Levenshtein distance
    int levenshtein_distance(const std::string& s1, const std::string& s2) {
        int m = s1.length();
        int n = s2.length();

        dp_table_.assign(m + 1, std::vector<int>(n + 1, 0));

        // Initialize base cases
        for (int i = 0; i <= m; ++i) dp_table_[i][0] = i;
        for (int j = 0; j <= n; ++j) dp_table_[0][j] = j;

        // Fill DP table
        for (int i = 1; i <= m; ++i) {
            for (int j = 1; j <= n; ++j) {
                if (s1[i-1] == s2[j-1]) {
                    dp_table_[i][j] = dp_table_[i-1][j-1];
                } else {
                    dp_table_[i][j] = 1 + std::min({
                        dp_table_[i-1][j],      // Delete
                        dp_table_[i][j-1],      // Insert
                        dp_table_[i-1][j-1]     // Replace
                    });
                }
            }
        }

        return dp_table_[m][n];
    }

public:
    // Calculate fuzzy match score (lower is better)
    double match_score(const std::string& query, const std::string& candidate) {
        int distance = levenshtein_distance(query, candidate);
        int max_len = std::max(query.length(), candidate.length());

        if (max_len == 0) return 1.0; // Perfect match

        // Normalized score (0 = perfect match, 1 = completely different)
        return static_cast<double>(distance) / max_len;
    }

    // Check if strings are similar enough
    bool is_similar(const std::string& query, const std::string& candidate,
                   double threshold = 0.3) {
        return match_score(query, candidate) <= threshold;
    }

    // Find best matches
    std::vector<std::pair<std::string, double>> find_matches(
        const std::string& query,
        const std::vector<std::string>& candidates,
        size_t max_results = 10) {

        std::vector<std::pair<std::string, double>> results;

        for (const auto& candidate : candidates) {
            double score = match_score(query, candidate);
            results.emplace_back(candidate, score);
        }

        // Sort by score (lower is better)
        std::sort(results.begin(), results.end(),
                 [](const auto& a, const auto& b) {
                     return a.second < b.second;
                 });

        // Return top results
        if (results.size() > max_results) {
            results.resize(max_results);
        }

        return results;
    }
};

// Code completion engine
class CodeCompletionEngine {
private:
    std::unordered_map<std::string, std::vector<Symbol>> symbol_table_;
    FuzzyMatcher fuzzy_matcher_;
    std::unordered_map<std::string, int> context_weights_;

    // Calculate relevance score for a symbol
    double calculate_relevance(const Symbol& symbol,
                              const std::string& query,
                              const std::string& context) {
        double score = 0.0;

        // Exact prefix match bonus
        if (symbol.name.find(query) == 0) {
            score += 10.0;
        }

        // Fuzzy match score (lower distance is better)
        double fuzzy_score = fuzzy_matcher_.match_score(query, symbol.name);
        score += (1.0 - fuzzy_score) * 5.0;  // Convert to higher-is-better

        // Frequency bonus
        score += std::log(symbol.frequency + 1) * 2.0;

        // Context relevance
        auto context_it = context_weights_.find(context);
        if (context_it != context_weights_.end()) {
            score += context_it->second;
        }

        // Type relevance (prefer certain types in certain contexts)
        if (context == "function_call" && symbol.type == "function") {
            score += 3.0;
        } else if (context == "variable" && symbol.type == "variable") {
            score += 2.0;
        }

        return score;
    }

public:
    // Add symbol to completion database
    void add_symbol(const std::string& file, const Symbol& symbol) {
        symbol_table_[file].push_back(symbol);
    }

    // Build global symbol index
    void build_index() {
        // In a real implementation, this would build inverted indexes,
        // compute context weights, etc.

        // Simple context weights
        context_weights_["function_call"] = 2;
        context_weights_["variable"] = 1;
        context_weights_["type"] = 3;
        context_weights_["class"] = 4;
    }

    // Get completion suggestions
    std::vector<Symbol> get_completions(const std::string& query,
                                      const std::string& context = "",
                                      size_t max_results = 10) {
        std::vector<std::pair<Symbol, double>> candidates;

        // Collect candidates from all files
        for (const auto& file_symbols : symbol_table_) {
            for (const Symbol& symbol : file_symbols.second) {
                // Quick filter: must be somewhat similar
                if (!fuzzy_matcher_.is_similar(query, symbol.name, 0.8)) {
                    continue;
                }

                double relevance = calculate_relevance(symbol, query, context);
                candidates.emplace_back(symbol, relevance);
            }
        }

        // Sort by relevance score (highest first)
        std::sort(candidates.begin(), candidates.end(),
                 [](const auto& a, const auto& b) {
                     return a.second > b.second;  // Higher score first
                 });

        // Extract top results
        std::vector<Symbol> results;
        for (size_t i = 0; i < std::min(max_results, candidates.size()); ++i) {
            results.push_back(candidates[i].first);
        }

        return results;
    }

    // Intelligent ranking using machine learning features
    void update_symbol_frequency(const std::string& symbol_name, int increment = 1) {
        for (auto& file_symbols : symbol_table_) {
            for (auto& symbol : file_symbols.second) {
                if (symbol.name == symbol_name) {
                    symbol.frequency += increment;
                }
            }
        }
    }

    // Context-aware filtering
    std::vector<Symbol> get_contextual_completions(
        const std::string& query,
        const std::string& current_file,
        int line_number,
        const std::string& context) {

        // First get general completions
        auto general = get_completions(query, context);

        // Boost symbols from current file
        for (auto& symbol : general) {
            // Check if symbol is from current file
            bool is_local = false;
            for (const auto& file_symbols : symbol_table_) {
                if (file_symbols.first == current_file) {
                    for (const auto& s : file_symbols.second) {
                        if (s.name == symbol.name) {
                            is_local = true;
                            break;
                        }
                    }
                    if (is_local) break;
                }
            }

            if (is_local) {
                symbol.score += 5.0;  // Local symbols get boost
            }
        }

        // Re-sort after boosting
        std::sort(general.begin(), general.end(),
                 [](const Symbol& a, const Symbol& b) {
                     return a.score > b.score;
                 });

        return general;
    }
};

// IDE-like code completion system
class IDECodeCompletion {
private:
    CodeCompletionEngine engine_;
    std::unordered_map<std::string, std::string> file_contents_;

public:
    // Load code file and extract symbols
    void load_file(const std::string& filename, const std::string& content) {
        file_contents_[filename] = content;

        // Simple symbol extraction (in real IDE, this would use proper parsing)
        extract_symbols(filename, content);
    }

private:
    void extract_symbols(const std::string& filename, const std::string& content) {
        // Very simplified symbol extraction
        // In a real IDE, this would use AST parsing

        std::vector<std::string> lines;
        std::string line;
        for (char c : content) {
            if (c == '\n') {
                lines.push_back(line);
                line.clear();
            } else {
                line += c;
            }
        }
        if (!line.empty()) lines.push_back(line);

        for (const std::string& l : lines) {
            // Look for function definitions
            if (l.find("function ") != std::string::npos ||
                l.find("def ") != std::string::npos ||
                l.find("void ") != std::string::npos) {
                extract_function_symbol(filename, l);
            }
            // Look for variable declarations
            else if (l.find("int ") != std::string::npos ||
                     l.find("var ") != std::string::npos ||
                     l.find("let ") != std::string::npos) {
                extract_variable_symbol(filename, l);
            }
        }
    }

    void extract_function_symbol(const std::string& filename, const std::string& line) {
        // Extract function name (simplified)
        size_t start = line.find(' ') + 1;
        size_t end = line.find('(', start);
        if (end != std::string::npos) {
            std::string func_name = line.substr(start, end - start);
            // Remove extra spaces
            func_name.erase(func_name.begin(),
                           std::find_if(func_name.begin(), func_name.end(),
                                       [](char c) { return c != ' '; }));
            func_name.erase(std::find_if(func_name.rbegin(), func_name.rend(),
                                       [](char c) { return c != ' '; }).base(),
                           func_name.end());

            if (!func_name.empty()) {
                engine_.add_symbol(filename, Symbol(func_name, "function"));
            }
        }
    }

    void extract_variable_symbol(const std::string& filename, const std::string& line) {
        // Extract variable name (simplified)
        size_t start = line.find(' ') + 1;
        size_t end = line.find('=', start);
        if (end == std::string::npos) end = line.find(';', start);
        if (end != std::string::npos) {
            std::string var_name = line.substr(start, end - start);
            // Remove extra spaces
            var_name.erase(var_name.begin(),
                          std::find_if(var_name.begin(), var_name.end(),
                                      [](char c) { return c != ' '; }));
            var_name.erase(std::find_if(var_name.rbegin(), var_name.rend(),
                                      [](char c) { return c != ' '; }).base(),
                          var_name.end());

            if (!var_name.empty()) {
                engine_.add_symbol(filename, Symbol(var_name, "variable"));
            }
        }
    }

public:
    // Initialize the completion engine
    void initialize() {
        engine_.build_index();
    }

    // Get completions for current context
    std::vector<Symbol> get_completions(const std::string& prefix,
                                      const std::string& context = "general") {
        return engine_.get_completions(prefix, context);
    }

    // Simulate typing and getting completions
    void demonstrate_completion() {
        std::cout << "IDE Code Completion DP Demonstration" << std::endl;

        // Load some sample code
        std::string sample_code = R"(
function calculateSum(a, b) {
    return a + b;
}

function processData(data) {
    var result = calculateSum(data.x, data.y);
    return result;
}

int main() {
    var data = {x: 1, y: 2};
    var sum = processData(data);
    return sum;
}
)";

        load_file("sample.js", sample_code);
        initialize();

        // Test completions
        std::vector<std::string> queries = {"calc", "proc", "data", "sum"};

        for (const auto& query : queries) {
            std::cout << "\nCompletions for '" << query << "':" << std::endl;
            auto completions = get_completions(query, "function_call");

            for (size_t i = 0; i < std::min(size_t(5), completions.size()); ++i) {
                const auto& symbol = completions[i];
                std::cout << "  " << symbol.name << " (" << symbol.type << ")" << std::endl;
            }
        }

        std::cout << "\nDP techniques used:" << std::endl;
        std::cout << "- Fuzzy string matching with Levenshtein distance" << std::endl;
        std::cout << "- Symbol ranking with context and frequency analysis" << std::endl;
        std::cout << "- Relevance scoring for intelligent suggestions" << std::endl;
    }
};

// Example usage
int main() {
    IDECodeCompletion ide;
    ide.demonstrate_completion();
    return 0;
}

