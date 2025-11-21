/*
 * Rush Hour Puzzle Backtracking - Game Development
 * 
 * Source: Rush Hour puzzle game, sliding block puzzles
 * Pattern: Backtracking for sliding block puzzle solving
 * 
 * What Makes It Ingenious:
 * - State space search: Explore all possible board configurations
 * - Move validation: Check if moves are legal
 * - Goal detection: Check if red car can exit
 * - Memoization: Avoid revisiting same states
 * - Used in Rush Hour, sliding block puzzles, puzzle games
 * 
 * When to Use:
 * - Sliding block puzzles
 * - Rush Hour game
 * - Puzzle game solvers
 * - Move validation systems
 * - Puzzle generation
 * 
 * Real-World Usage:
 * - Rush Hour puzzle games
 * - Sliding block puzzle solvers
 * - Puzzle game engines
 * - Educational puzzle games
 * 
 * Time Complexity: O(b^d) where b is branching factor, d is depth
 * Space Complexity: O(d) for recursion, O(n) for state storage
 */

#include <vector>
#include <string>
#include <unordered_set>
#include <iostream>
#include <algorithm>

class RushHourBacktracking {
public:
    enum class Direction { HORIZONTAL, VERTICAL };
    
    // Car/Vehicle in puzzle
    struct Car {
        int id;
        int row, col;
        int length;
        Direction direction;
        char symbol;
        
        Car(int i, int r, int c, int len, Direction dir, char sym)
            : id(i), row(r), col(c), length(len), direction(dir), symbol(sym) {}
        
        // Get all positions occupied by car
        std::vector<std::pair<int, int>> get_positions() const {
            std::vector<std::pair<int, int>> positions;
            if (direction == Direction::HORIZONTAL) {
                for (int i = 0; i < length; i++) {
                    positions.push_back({row, col + i});
                }
            } else {
                for (int i = 0; i < length; i++) {
                    positions.push_back({row + i, col});
                }
            }
            return positions;
        }
    };
    
    // Rush Hour board state
    class RushHourBoard {
    private:
        static const int SIZE = 6;
        std::vector<std::vector<char>> grid_;
        std::vector<Car> cars_;
        int red_car_id_;
        
        bool is_valid_position(const Car& car) const {
            auto positions = car.get_positions();
            for (const auto& [r, c] : positions) {
                if (r < 0 || r >= SIZE || c < 0 || c >= SIZE) {
                    return false;
                }
                // Check if position is empty or occupied by this car
                if (grid_[r][c] != '.' && grid_[r][c] != car.symbol) {
                    return false;
                }
            }
            return true;
        }
        
        void update_grid() {
            // Clear grid
            for (int r = 0; r < SIZE; r++) {
                for (int c = 0; c < SIZE; c++) {
                    grid_[r][c] = '.';
                }
            }
            
            // Place cars
            for (const auto& car : cars_) {
                auto positions = car.get_positions();
                for (const auto& [r, c] : positions) {
                    grid_[r][c] = car.symbol;
                }
            }
        }
        
        std::string get_state_hash() const {
            std::string hash;
            for (const auto& car : cars_) {
                hash += std::to_string(car.id) + "," + 
                       std::to_string(car.row) + "," + 
                       std::to_string(car.col) + ";";
            }
            return hash;
        }
        
    public:
        RushHourBoard() {
            grid_.resize(SIZE, std::vector<char>(SIZE, '.'));
        }
        
        void add_car(const Car& car) {
            cars_.push_back(car);
            if (car.symbol == 'R') {
                red_car_id_ = car.id;
            }
            update_grid();
        }
        
        bool is_solved() const {
            // Check if red car (horizontal) can exit right
            const Car& red_car = cars_[red_car_id_];
            if (red_car.direction == Direction::HORIZONTAL && 
                red_car.col + red_car.length == SIZE) {
                return true;
            }
            return false;
        }
        
        bool move_car(int car_id, int delta) {
            Car& car = cars_[car_id];
            Car original = car;
            
            if (car.direction == Direction::HORIZONTAL) {
                car.col += delta;
            } else {
                car.row += delta;
            }
            
            if (is_valid_position(car)) {
                update_grid();
                return true;
            } else {
                car = original;  // Revert
                return false;
            }
        }
        
        std::vector<std::pair<int, int>> get_possible_moves() const {
            std::vector<std::pair<int, int>> moves;
            
            for (size_t i = 0; i < cars_.size(); i++) {
                // Try moving forward
                RushHourBoard test_board = *this;
                if (test_board.move_car(i, 1)) {
                    moves.push_back({static_cast<int>(i), 1});
                }
                
                // Try moving backward
                test_board = *this;
                if (test_board.move_car(i, -1)) {
                    moves.push_back({static_cast<int>(i), -1});
                }
            }
            
            return moves;
        }
        
        std::string get_state() const {
            return get_state_hash();
        }
        
        void print() const {
            for (int r = 0; r < SIZE; r++) {
                for (int c = 0; c < SIZE; c++) {
                    std::cout << grid_[r][c] << " ";
                }
                std::cout << std::endl;
            }
        }
    };
    
    // Rush Hour solver with backtracking
    class RushHourSolver {
    private:
        std::unordered_set<std::string> visited_states_;
        std::vector<std::pair<int, int>> solution_path_;
        int max_depth_;
        
        bool solve_recursive(RushHourBoard& board, int depth) {
            if (depth > max_depth_) {
                return false;
            }
            
            // Check if solved
            if (board.is_solved()) {
                return true;
            }
            
            // Check if state already visited
            std::string state = board.get_state();
            if (visited_states_.find(state) != visited_states_.end()) {
                return false;  // Already explored
            }
            visited_states_.insert(state);
            
            // Try all possible moves
            auto moves = board.get_possible_moves();
            for (const auto& [car_id, delta] : moves) {
                RushHourBoard next_board = board;
                if (next_board.move_car(car_id, delta)) {
                    solution_path_.push_back({car_id, delta});
                    
                    if (solve_recursive(next_board, depth + 1)) {
                        return true;
                    }
                    
                    // Backtrack
                    solution_path_.pop_back();
                }
            }
            
            return false;
        }
        
    public:
        RushHourSolver(int max_depth = 50) : max_depth_(max_depth) {}
        
        bool solve(RushHourBoard board) {
            visited_states_.clear();
            solution_path_.clear();
            return solve_recursive(board, 0);
        }
        
        std::vector<std::pair<int, int>> get_solution() const {
            return solution_path_;
        }
        
        int get_move_count() const {
            return solution_path_.size();
        }
    };
};

// Example usage
int main() {
    using namespace RushHourBacktracking;
    
    // Create Rush Hour board
    RushHourBoard board;
    
    // Add red car (horizontal, length 2)
    board.add_car(Car(0, 2, 0, 2, Direction::HORIZONTAL, 'R'));
    
    // Add blocking cars
    board.add_car(Car(1, 0, 2, 2, Direction::VERTICAL, 'A'));
    board.add_car(Car(2, 1, 4, 2, Direction::VERTICAL, 'B'));
    board.add_car(Car(3, 4, 3, 2, Direction::HORIZONTAL, 'C'));
    
    std::cout << "Initial board:" << std::endl;
    board.print();
    
    // Solve
    RushHourSolver solver(20);
    if (solver.solve(board)) {
        std::cout << "\nSolution found in " << solver.get_move_count() << " moves!" << std::endl;
        auto solution = solver.get_solution();
        for (size_t i = 0; i < solution.size(); i++) {
            std::cout << "Move " << i + 1 << ": Car " << solution[i].first 
                      << " move " << (solution[i].second > 0 ? "forward" : "backward") << std::endl;
        }
    } else {
        std::cout << "\nNo solution found within depth limit" << std::endl;
    }
    
    return 0;
}

