/*
 * Puzzle Generation with Backtracking - Game Development
 * 
 * Source: Puzzle game generation algorithms
 * Pattern: Backtracking to generate valid puzzles
 * 
 * What Makes It Ingenious:
 * - Generate and validate: Create puzzle and verify it's solvable
 * - Uniqueness checking: Ensure puzzle has unique solution
 * - Difficulty control: Control puzzle difficulty
 * - Constraint satisfaction: Generate puzzles satisfying constraints
 * - Used in Sudoku generators, crossword generators, puzzle games
 * 
 * When to Use:
 * - Puzzle generation
 * - Sudoku generation
 * - Crossword generation
 * - Puzzle validation
 * - Difficulty control
 * 
 * Real-World Usage:
 * - Sudoku generators
 * - Crossword generators
 * - Puzzle game engines
 * - Educational puzzle software
 * - Procedural puzzle generation
 * 
 * Time Complexity: O(9^m) for Sudoku where m is cells to remove
 * Space Complexity: O(n) for puzzle storage
 */

#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include <unordered_set>

class PuzzleGenerationBacktracking {
public:
    // Sudoku puzzle generator
    class SudokuGenerator {
    private:
        static const int SIZE = 9;
        static const int BOX_SIZE = 3;
        std::vector<std::vector<int>> grid_;
        std::mt19937 rng_;
        
        bool is_valid(int row, int col, int num) {
            // Check row
            for (int c = 0; c < SIZE; c++) {
                if (grid_[row][c] == num) return false;
            }
            
            // Check column
            for (int r = 0; r < SIZE; r++) {
                if (grid_[r][col] == num) return false;
            }
            
            // Check box
            int box_row = (row / BOX_SIZE) * BOX_SIZE;
            int box_col = (col / BOX_SIZE) * BOX_SIZE;
            for (int r = box_row; r < box_row + BOX_SIZE; r++) {
                for (int c = box_col; c < box_col + BOX_SIZE; c++) {
                    if (grid_[r][c] == num) return false;
                }
            }
            
            return true;
        }
        
        bool solve_recursive() {
            for (int r = 0; r < SIZE; r++) {
                for (int c = 0; c < SIZE; c++) {
                    if (grid_[r][c] == 0) {
                        // Try numbers in random order
                        std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9};
                        std::shuffle(numbers.begin(), numbers.end(), rng_);
                        
                        for (int num : numbers) {
                            if (is_valid(r, c, num)) {
                                grid_[r][c] = num;
                                if (solve_recursive()) {
                                    return true;
                                }
                                grid_[r][c] = 0;  // Backtrack
                            }
                        }
                        return false;
                    }
                }
            }
            return true;  // Solved
        }
        
        int count_solutions(int& count) {
            for (int r = 0; r < SIZE; r++) {
                for (int c = 0; c < SIZE; c++) {
                    if (grid_[r][c] == 0) {
                        for (int num = 1; num <= 9; num++) {
                            if (is_valid(r, c, num)) {
                                grid_[r][c] = num;
                                count_solutions(count);
                                grid_[r][c] = 0;
                                
                                if (count > 1) return count;  // Early exit
                            }
                        }
                        return count;
                    }
                }
            }
            count++;
            return count;
        }
        
    public:
        SudokuGenerator(int seed = 0) : rng_(seed) {
            grid_.resize(SIZE, std::vector<int>(SIZE, 0));
        }
        
        // Generate complete valid Sudoku
        void generate_complete() {
            solve_recursive();
        }
        
        // Remove numbers to create puzzle
        std::vector<std::vector<int>> generate_puzzle(int num_clues) {
            generate_complete();
            std::vector<std::vector<int>> puzzle = grid_;
            
            // Create list of all positions
            std::vector<std::pair<int, int>> positions;
            for (int r = 0; r < SIZE; r++) {
                for (int c = 0; c < SIZE; c++) {
                    positions.push_back({r, c});
                }
            }
            std::shuffle(positions.begin(), positions.end(), rng_);
            
            // Remove numbers while maintaining unique solution
            int removed = 0;
            int target_remove = SIZE * SIZE - num_clues;
            
            for (const auto& [r, c] : positions) {
                if (removed >= target_remove) break;
                
                int saved = puzzle[r][c];
                puzzle[r][c] = 0;
                
                // Check if still has unique solution
                std::vector<std::vector<int>> test_puzzle = puzzle;
                int solution_count = 0;
                SudokuGenerator test_gen;
                test_gen.grid_ = test_puzzle;
                test_gen.count_solutions(solution_count);
                
                if (solution_count == 1) {
                    removed++;
                } else {
                    puzzle[r][c] = saved;  // Keep this clue
                }
            }
            
            return puzzle;
        }
        
        std::vector<std::vector<int>> get_grid() const {
            return grid_;
        }
    };
    
    // Crossword puzzle generator (simplified)
    class CrosswordGenerator {
    private:
        int rows_, cols_;
        std::vector<std::vector<char>> grid_;
        std::vector<std::string> words_;
        std::mt19937 rng_;
        
        bool can_place_word(const std::string& word, int row, int col, bool horizontal) {
            if (horizontal) {
                if (col + word.length() > cols_) return false;
                for (size_t i = 0; i < word.length(); i++) {
                    if (grid_[row][col + i] != '.' && grid_[row][col + i] != word[i]) {
                        return false;
                    }
                }
            } else {
                if (row + word.length() > rows_) return false;
                for (size_t i = 0; i < word.length(); i++) {
                    if (grid_[row + i][col] != '.' && grid_[row + i][col] != word[i]) {
                        return false;
                    }
                }
            }
            return true;
        }
        
        void place_word(const std::string& word, int row, int col, bool horizontal) {
            if (horizontal) {
                for (size_t i = 0; i < word.length(); i++) {
                    grid_[row][col + i] = word[i];
                }
            } else {
                for (size_t i = 0; i < word.length(); i++) {
                    grid_[row + i][col] = word[i];
                }
            }
        }
        
        bool generate_recursive(size_t word_index) {
            if (word_index >= words_.size()) {
                return true;  // All words placed
            }
            
            const std::string& word = words_[word_index];
            
            // Try placing word in all possible positions
            for (int r = 0; r < rows_; r++) {
                for (int c = 0; c < cols_; c++) {
                    // Try horizontal
                    if (can_place_word(word, r, c, true)) {
                        std::vector<std::vector<char>> saved = grid_;
                        place_word(word, r, c, true);
                        
                        if (generate_recursive(word_index + 1)) {
                            return true;
                        }
                        
                        grid_ = saved;  // Backtrack
                    }
                    
                    // Try vertical
                    if (can_place_word(word, r, c, false)) {
                        std::vector<std::vector<char>> saved = grid_;
                        place_word(word, r, c, false);
                        
                        if (generate_recursive(word_index + 1)) {
                            return true;
                        }
                        
                        grid_ = saved;  // Backtrack
                    }
                }
            }
            
            return false;
        }
        
    public:
        CrosswordGenerator(int rows, int cols, const std::vector<std::string>& words, int seed = 0)
            : rows_(rows), cols_(cols), words_(words), rng_(seed) {
            grid_.resize(rows_, std::vector<char>(cols_, '.'));
        }
        
        bool generate() {
            // Sort words by length (longest first) for better placement
            std::sort(words_.begin(), words_.end(),
                     [](const std::string& a, const std::string& b) {
                         return a.length() > b.length();
                     });
            
            return generate_recursive(0);
        }
        
        std::vector<std::vector<char>> get_grid() const {
            return grid_;
        }
        
        void print() const {
            for (int r = 0; r < rows_; r++) {
                for (int c = 0; c < cols_; c++) {
                    std::cout << grid_[r][c] << " ";
                }
                std::cout << std::endl;
            }
        }
    };
};

// Example usage
int main() {
    using namespace PuzzleGenerationBacktracking;
    
    // Generate Sudoku puzzle
    SudokuGenerator sudoku_gen(12345);
    auto puzzle = sudoku_gen.generate_puzzle(30);  // 30 clues
    
    std::cout << "Generated Sudoku puzzle (30 clues):" << std::endl;
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            std::cout << puzzle[r][c] << " ";
        }
        std::cout << std::endl;
    }
    
    // Generate crossword
    std::vector<std::string> words = {"HELLO", "WORLD", "GAME", "CODE"};
    CrosswordGenerator crossword(10, 10, words, 54321);
    if (crossword.generate()) {
        std::cout << "\nGenerated crossword:" << std::endl;
        crossword.print();
    } else {
        std::cout << "\nCould not generate crossword with given words" << std::endl;
    }
    
    return 0;
}
