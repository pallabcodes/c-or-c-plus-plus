// Graham Scan Algorithm: Find convex hull of set of points
// Uses sorting and stack to build convex hull
// Time: O(n log n)
// Space: O(n)

#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>

using namespace std;

struct Point {
    int x, y;
    
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    
    Point operator-(const Point& other) const {
        return Point(x - other.x, y - other.y);
    }
    
    int cross(const Point& other) const {
        return x * other.y - y * other.x;
    }
    
    int distSq() const {
        return x * x + y * y;
    }
    
    bool operator<(const Point& other) const {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
};

// Orientation: 0 = collinear, 1 = clockwise, 2 = counterclockwise
int orientation(const Point& p1, const Point& p2, const Point& p3) {
    int val = (p2.y - p1.y) * (p3.x - p2.x) - (p2.x - p1.x) * (p3.y - p2.y);
    if (val == 0) return 0;
    return (val > 0) ? 1 : 2;
}

// Graham Scan for convex hull
vector<Point> grahamScan(vector<Point> points) {
    int n = points.size();
    if (n < 3) return points;
    
    // Find bottom-most point (or leftmost in case of tie)
    int bottom = 0;
    for (int i = 1; i < n; i++) {
        if (points[i].y < points[bottom].y ||
            (points[i].y == points[bottom].y && points[i].x < points[bottom].x)) {
            bottom = i;
        }
    }
    
    swap(points[0], points[bottom]);
    
    // Sort points by polar angle with respect to bottom point
    Point p0 = points[0];
    sort(points.begin() + 1, points.end(), [&p0](const Point& a, const Point& b) {
        int o = orientation(p0, a, b);
        if (o == 0) {
            return (p0 - a).distSq() < (p0 - b).distSq();
        }
        return o == 2;
    });
    
    // Build convex hull
    vector<Point> hull;
    hull.push_back(points[0]);
    hull.push_back(points[1]);
    
    for (int i = 2; i < n; i++) {
        while (hull.size() > 1 && 
               orientation(hull[hull.size() - 2], hull[hull.size() - 1], points[i]) != 2) {
            hull.pop_back();
        }
        hull.push_back(points[i]);
    }
    
    return hull;
}

// Example usage
int main() {
    vector<Point> points = {
        {0, 3}, {2, 2}, {1, 1}, {2, 1},
        {3, 0}, {0, 0}, {3, 3}
    };
    
    vector<Point> hull = grahamScan(points);
    
    cout << "Convex Hull (Graham Scan):" << endl;
    for (const Point& p : hull) {
        cout << "(" << p.x << ", " << p.y << ") ";
    }
    cout << endl;
    
    return 0;
}

