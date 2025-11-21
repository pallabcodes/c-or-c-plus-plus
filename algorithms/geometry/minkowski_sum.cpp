// Minkowski Sum: Sum of two convex polygons
// Used in computational geometry and collision detection
// Time: O(n + m) where n and m are polygon sizes
// Space: O(n + m)
// God modded implementation for geometric operations

#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>

struct Point {
    double x, y;
    
    Point(double x = 0, double y = 0) : x(x), y(y) {}
    
    Point operator+(const Point& other) const {
        return Point(x + other.x, y + other.y);
    }
    
    Point operator-(const Point& other) const {
        return Point(x - other.x, y - other.y);
    }
    
    bool operator<(const Point& other) const {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
    
    double cross(const Point& other) const {
        return x * other.y - y * other.x;
    }
    
    double dist2() const {
        return x * x + y * y;
    }
};

double cross(const Point& p0, const Point& p1, const Point& p2) {
    return (p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y);
}

// Build convex hull
std::vector<Point> convexHull(std::vector<Point> points) {
    int n = points.size();
    if (n < 3) return points;
    
    std::sort(points.begin(), points.end());
    
    std::vector<Point> hull;
    
    for (int i = 0; i < n; i++) {
        while (hull.size() >= 2 && 
               cross(hull[hull.size() - 2], hull.back(), points[i]) <= 0) {
            hull.pop_back();
        }
        hull.push_back(points[i]);
    }
    
    int lowerSize = hull.size();
    
    for (int i = n - 2; i >= 0; i--) {
        while (hull.size() > lowerSize && 
               cross(hull[hull.size() - 2], hull.back(), points[i]) <= 0) {
            hull.pop_back();
        }
        hull.push_back(points[i]);
    }
    
    hull.pop_back();
    return hull;
}

// Minkowski sum of two convex polygons
std::vector<Point> minkowskiSum(const std::vector<Point>& poly1, 
                                 const std::vector<Point>& poly2) {
    int n = poly1.size();
    int m = poly2.size();
    
    if (n == 0) return poly2;
    if (m == 0) return poly1;
    
    std::vector<Point> result;
    
    int i = 0, j = 0;
    
    for (int k = 0; k < n + m; k++) {
        result.push_back(poly1[i] + poly2[j]);
        
        Point v1 = poly1[(i + 1) % n] - poly1[i];
        Point v2 = poly2[(j + 1) % m] - poly2[j];
        
        double crossProd = v1.cross(v2);
        
        if (crossProd > 0 || (crossProd == 0 && v1.dist2() > v2.dist2())) {
            i = (i + 1) % n;
        } else {
            j = (j + 1) % m;
        }
    }
    
    return convexHull(result);
}

// Minkowski difference (used for collision detection)
std::vector<Point> minkowskiDifference(const std::vector<Point>& poly1, 
                                       const std::vector<Point>& poly2) {
    std::vector<Point> negPoly2;
    for (const Point& p : poly2) {
        negPoly2.push_back(Point(-p.x, -p.y));
    }
    
    std::reverse(negPoly2.begin(), negPoly2.end());
    
    return minkowskiSum(poly1, negPoly2);
}

// Check if point is inside polygon (for collision detection)
bool pointInPolygon(const Point& p, const std::vector<Point>& poly) {
    int n = poly.size();
    bool inside = false;
    
    for (int i = 0, j = n - 1; i < n; j = i++) {
        if (((poly[i].y > p.y) != (poly[j].y > p.y)) &&
            (p.x < (poly[j].x - poly[i].x) * (p.y - poly[i].y) / 
                   (poly[j].y - poly[i].y) + poly[i].x)) {
            inside = !inside;
        }
    }
    
    return inside;
}

// Collision detection using Minkowski difference
bool polygonsCollide(const std::vector<Point>& poly1, 
                     const std::vector<Point>& poly2) {
    std::vector<Point> diff = minkowskiDifference(poly1, poly2);
    Point origin(0, 0);
    return pointInPolygon(origin, diff);
}

// Example usage
int main() {
    std::vector<Point> poly1 = {
        {0, 0}, {2, 0}, {2, 2}, {0, 2}
    };
    
    std::vector<Point> poly2 = {
        {1, 1}, {3, 1}, {3, 3}, {1, 3}
    };
    
    std::vector<Point> sum = minkowskiSum(poly1, poly2);
    
    std::cout << "Minkowski sum has " << sum.size() << " points" << std::endl;
    for (const Point& p : sum) {
        std::cout << "(" << p.x << ", " << p.y << ") ";
    }
    std::cout << std::endl;
    
    bool collide = polygonsCollide(poly1, poly2);
    std::cout << "\nPolygons collide: " << (collide ? "Yes" : "No") << std::endl;
    
    return 0;
}

