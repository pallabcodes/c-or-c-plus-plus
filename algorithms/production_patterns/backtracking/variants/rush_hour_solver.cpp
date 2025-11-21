/*
 * Rush Hour Puzzle Solver with Backtracking - Game Development
 * 
 * Source: Rush Hour puzzle game, sliding block puzzles
 * Pattern: Backtracking to solve sliding block puzzles
 * 
 * What Makes It Ingenious:
 * - Move generation: Generate all possible moves
 * - State representation: Efficient board state encoding
 * - Backtracking: Undo moves when stuck
 * - Goal checking: Check if red car can exit
 * - Used in puzzle games, sliding block puzzles
 * 
 * When to Use:
 * - Sliding block puzzle games
 * - Rush Hour style puzzles
 * - Puzzle game solvers
 * - Move validation systems
 * 
 * Real-World Usage:
 * - Rush Hour puzzle games
 * - Sliding block puzzle solvers
 * - Puzzle game engines
 * - Educational puzzle games
 * 
 * Time Complexity: O(b^d) where b is branching factor, d is depth
 * Space Complexity: O(d) for move history
 */

#include <vector>
#include <string>
#include <unordered_set>
#include <iostream>
#include <algorithm>

class RushHourSolver {
public:
    enum class Direction { HORIZONTAL, VERTICAL };
    
    struct Car {
        int id;
        int row, col;
        int length;
        Direction direction;
        char symbol;
        
        Car(int i, int r, int c, int len, Direction dir, char sym)
            : id(i), row(r), col(c), length(len), direction(dir), symbol(sym) {}
        
        bool operator==(const Car& other) const {
            return id == other.id && row == other.row && col == other.col;
        }
    };
    
    struct BoardState {
        std::vector<Car> cars;
        int moves;
        
        std::string to_string() const {
            std::string result;
            for (const auto& car : cars) {
                result += std::to_string(car.id) + "," + 
                         std::to_string(car.row) + "," + 
                         std::to_string(car.col) + ";";
            }
            return result;
        }
    };
    
    class RushHourPuzzle {
    private:
        static const int BOARD_SIZE = 6;
        std::vector<Car> cars_;
        int red_car_id_;
        std::unordered_set<std::string> visited_states_;
        
        // Check if move is valid
        bool is_valid_move(const Car& car, int new_row, int new_col) {
            // Check bounds
            if (new_row < 0 || new_col < 0) return false;
            
            if (car.direction == Direction::HORIZONTAL) {
                if (new_col + car.length > BOARD_SIZE) return false;
            } else {
                if (new_row + car.length > BOARD_SIZE) return false;
            }
            
            // Check collisions
            for (const auto& other : cars_) {
                if (other.id == car.id) continue;
                
                // Check if car overlaps with other
                if (car.direction == Direction::HORIZONTAL) {
                    for (int c = new_col; c < new_col + car.length; c++) {
                        if (other.direction == Direction::HORIZONTAL) {
                            if (car.row == other.row && 
                                c >= other.col && c < other.col + other.length) {
                                return false;
                            }
                        } else {
                            if (c == other.col &&
                                car.row >= other.row && 
                                car.row < other.row + other.length) {
                                return false;
                            }
                        }
                    }
                } else {
                    for (int r = new_row; r < new_row + car.length; r++) {
                        if (other.direction == Direction::VERTICAL) {
                            if (car.col == other.col &&
                                r >= other.row && r < other.row + other.length) {
                                return false;
                            }
                        } else {
                            if (r == other.row &&
                                car.col >= other.col &&
                                car.col < other.col + other.length) {
                                return false;
                            }
                        }
                    }
                }
            }
            
            return true;
        }
        
        // Check if puzzle is solved (red car can exit)
        bool is_solved() {
            for (const auto& car : cars_) {
                if (car.id == red_car_id_) {
                    // Red car is horizontal, check if at right edge
                    return car.direction == Direction::HORIZONTAL &&
                           car.col + car.length == BOARD_SIZE;
                }
            }
            return false;
        }
        
        // Generate all possible moves
        std::vector<std::pair<int, std::pair<int, int>>> generate_moves() {
            std::vector<std::pair<int, std::pair<int, int>>> moves;
            
            for (size_t i = 0; i < cars_.size(); i++) {
                const auto& car = cars_[i];
                
                if (car.direction == Direction::HORIZONTAL) {
                    // Try moving left
                    if (is_valid_move(car, car.row, car.col - 1)) {
                        moves.push_back({static_cast<int>(i), {car.row, car.col - 1}});
                    }
                    // Try moving right
                    if (is_valid_move(car, car.row, car.col + 1)) {
                        moves.push_back({static_cast<int>(i), {car.row, car.col + 1}});
                    }
                } else {
                    // Try moving up
                    if (is_valid_move(car, car.row - 1, car.col)) {
                        moves.push_back({static_cast<int>(i), {car.row - 1, car.col}});
                    }
                    // Try moving down
                    if (is_valid_move(car, car.row + 1, car.col)) {
                        moves.push_back({static_cast<int>(i), {car.row + 1, car.col}});
                    }
                }
            }
            
            return moves;
        }
        
        // Apply move
        void apply_move(int car_index, int new_row, int new_col) {
            cars_[car_index].row = new_row;
            cars_[car_index].col = new_col;
        }
        
        // Backtracking search
        bool solve_recursive(int depth, int max_depth) {
            if (depth > max_depth) {
                return false;
            }
            
            if (is_solved()) {
                return true;
            }
            
            std::string state_str = get_state_string();
            if (visited_states_.find(state_str) != visited_states_.end()) {
                return false;  // Already visited
            }
            visited_states_.insert(state_str);
            
            auto moves = generate_moves();
            for (const auto& [car_index, new_pos] : moves) {
                // Save state
                int old_row = cars_[car_index].row;
                int old_col = cars_[car_index].col;
                
                // Apply move
                apply_move(car_index, new_pos.first, new_pos.second);
                
                // Recursively solve
                if (solve_recursive(depth + 1, max_depth)) {
                    return true;
                }
                
                // Backtrack
                apply_move(car_index, old_row, old_col);
            }
            
            visited_states_.erase(state_str);
            return false;
        }
        
        std::string get_state_string() const {
            BoardState state;
            state.cars = cars_;
            return state.to_string();
        }
        
    public:
        RushHourPuzzle(const std::vector<Car>& cars, int red_car_id)
            : cars_(cars), red_car_id_(red_car_id) {}
        
        bool solve(int max_depth = 50) {
            visited_states_.clear();
            return solve_recursive(0, max_depth);
        }
        
        std::vector<Car> get_cars() const {
            return cars_;
        }
    };
};

// Example usage
int main() {
    using namespace RushHourSolver;
    
    // Create Rush Hour puzzle
    // Red car (id=0) horizontal at row 2, needs to exit right
    std::vector<Car> cars = {
        Car(0, 2, 0, 2, Direction::HORIZONTAL, 'R'),  // Red car
        Car(1, 0, 0, 2, Direction::VERTICAL, 'A'),
        Car(2, 1, 2, 2, Direction::VERTICAL, 'B'),
        Car(3, 4, 3, 2, Direction::HORIZONTAL, 'C')
    };
    
    RushHourPuzzle puzzle(cars, 0);
    
    if (puzzle.solve()) {
        std::cout << "Puzzle solved!" << std::endl;
        auto solution = puzzle.get_cars();
        for (const auto& car : solution) {
            std::cout << "Car " << car.id << " at (" << car.row << ", " 
                      << car.col << ")" << std::endl;
        }
    } else {
        std::cout << "Puzzle unsolvable or too complex" << std::endl;
    }
    
    return 0;
}

