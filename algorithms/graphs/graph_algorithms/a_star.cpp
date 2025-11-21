// A* Search Algorithm: Informed search algorithm for pathfinding
// Uses heuristic function to guide search towards goal
// Optimal when heuristic is admissible (never overestimates)
// Time: O(b^d) where b is branching factor, d is depth
// Space: O(b^d)

#include <vector>
#include <queue>
#include <unordered_map>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <functional>

struct Node {
    int x, y;
    int g; // Cost from start
    int h; // Heuristic cost to goal
    int f; // Total cost (g + h)
    Node* parent;
    
    Node(int x, int y) : x(x), y(y), g(0), h(0), f(0), parent(nullptr) {}
    
    bool operator>(const Node& other) const {
        return f > other.f;
    }
};

// Manhattan distance heuristic
int manhattanDistance(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

// Euclidean distance heuristic
double euclideanDistance(int x1, int y1, int x2, int y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

// A* pathfinding algorithm
std::vector<std::pair<int, int>> aStar(
    const std::vector<std::vector<int>>& grid,
    std::pair<int, int> start,
    std::pair<int, int> goal) {
    
    int rows = grid.size();
    int cols = grid[0].size();
    
    // Directions: up, down, left, right, and diagonals
    int dx[] = {-1, 1, 0, 0, -1, -1, 1, 1};
    int dy[] = {0, 0, -1, 1, -1, 1, -1, 1};
    int costs[] = {1, 1, 1, 1, 1.414, 1.414, 1.414, 1.414};
    
    std::priority_queue<Node*, std::vector<Node*>, 
                       std::function<bool(Node*, Node*)>> pq(
        [](Node* a, Node* b) { return a->f > b->f; });
    
    std::vector<std::vector<bool>> closed(rows, std::vector<bool>(cols, false));
    std::vector<std::vector<Node*>> nodes(rows, std::vector<Node*>(cols, nullptr));
    
    Node* startNode = new Node(start.first, start.second);
    startNode->h = manhattanDistance(start.first, start.second, goal.first, goal.second);
    startNode->f = startNode->g + startNode->h;
    nodes[start.first][start.second] = startNode;
    pq.push(startNode);
    
    while (!pq.empty()) {
        Node* current = pq.top();
        pq.pop();
        
        int x = current->x;
        int y = current->y;
        
        if (closed[x][y]) continue;
        closed[x][y] = true;
        
        if (x == goal.first && y == goal.second) {
            // Reconstruct path
            std::vector<std::pair<int, int>> path;
            Node* node = current;
            while (node != nullptr) {
                path.push_back({node->x, node->y});
                node = node->parent;
            }
            std::reverse(path.begin(), path.end());
            
            // Cleanup
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    if (nodes[i][j]) delete nodes[i][j];
                }
            }
            
            return path;
        }
        
        for (int i = 0; i < 8; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            
            if (nx < 0 || nx >= rows || ny < 0 || ny >= cols) continue;
            if (grid[nx][ny] == 1) continue; // Obstacle
            if (closed[nx][ny]) continue;
            
            int newG = current->g + costs[i];
            
            if (nodes[nx][ny] == nullptr) {
                nodes[nx][ny] = new Node(nx, ny);
            }
            
            if (newG < nodes[nx][ny]->g || nodes[nx][ny]->g == 0) {
                nodes[nx][ny]->g = newG;
                nodes[nx][ny]->h = manhattanDistance(nx, ny, goal.first, goal.second);
                nodes[nx][ny]->f = nodes[nx][ny]->g + nodes[nx][ny]->h;
                nodes[nx][ny]->parent = current;
                pq.push(nodes[nx][ny]);
            }
        }
    }
    
    // Cleanup
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (nodes[i][j]) delete nodes[i][j];
        }
    }
    
    return {}; // No path found
}

// Example usage
int main() {
    // 0 = free, 1 = obstacle
    std::vector<std::vector<int>> grid = {
        {0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0},
        {0, 0, 0, 0, 0, 0}
    };
    
    std::pair<int, int> start = {0, 0};
    std::pair<int, int> goal = {4, 5};
    
    std::vector<std::pair<int, int>> path = aStar(grid, start, goal);
    
    if (path.empty()) {
        std::cout << "No path found!" << std::endl;
    } else {
        std::cout << "Path found:" << std::endl;
        for (auto [x, y] : path) {
            std::cout << "(" << x << ", " << y << ") ";
        }
        std::cout << std::endl;
    }
    
    return 0;
}

