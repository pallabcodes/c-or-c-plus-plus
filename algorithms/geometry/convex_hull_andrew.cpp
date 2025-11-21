// Andrew's Monotone Chain Algorithm: Alternative convex hull algorithm
// Processes points in sorted order, builds upper and lower hulls separately
// Time: O(n log n)
// Space: O(n)

#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

struct Point {
    int x, y;
    
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    
    bool operator<(const Point& other) const {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
    
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

int cross(const Point& O, const Point& A, const Point& B) {
    return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

// Andrew's Monotone Chain
vector<Point> andrewMonotoneChain(vector<Point> points) {
    int n = points.size();
    if (n <= 1) return points;
    
    sort(points.begin(), points.end());
    
    vector<Point> hull;
    hull.reserve(n + 1);
    
    // Build lower hull
    for (int i = 0; i < n; i++) {
        while (hull.size() >= 2 && 
               cross(hull[hull.size() - 2], hull[hull.size() - 1], points[i]) <= 0) {
            hull.pop_back();
        }
        hull.push_back(points[i]);
    }
    
    // Build upper hull
    int lowerSize = hull.size();
    for (int i = n - 2; i >= 0; i--) {
        while (hull.size() > lowerSize && 
               cross(hull[hull.size() - 2], hull[hull.size() - 1], points[i]) <= 0) {
            hull.pop_back();
        }
        hull.push_back(points[i]);
    }
    
    // Remove duplicate point
    if (hull.size() > 1 && hull[0] == hull.back()) {
        hull.pop_back();
    }
    
    return hull;
}

// Example usage
int main() {
    vector<Point> points = {
        {0, 3}, {2, 2}, {1, 1}, {2, 1},
        {3, 0}, {0, 0}, {3, 3}
    };
    
    vector<Point> hull = andrewMonotoneChain(points);
    
    cout << "Convex Hull (Andrew's Monotone Chain):" << endl;
    for (const Point& p : hull) {
        cout << "(" << p.x << ", " << p.y << ") ";
    }
    cout << endl;
    
    return 0;
}

