// Rotating Calipers Algorithm
// Finds diameter of convex polygon, width, minimum area bounding rectangle
// Based on computational geometry research
// Time: O(n)
// Space: O(n)
// God modded implementation for geometric optimization

#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <climits>

struct Point {
    double x, y;
    
    Point(double x = 0, double y = 0) : x(x), y(y) {}
    
    Point operator-(const Point& other) const {
        return Point(x - other.x, y - other.y);
    }
    
    Point operator+(const Point& other) const {
        return Point(x + other.x, y + other.y);
    }
    
    double cross(const Point& other) const {
        return x * other.y - y * other.x;
    }
    
    double dot(const Point& other) const {
        return x * other.x + y * other.y;
    }
    
    double dist2() const {
        return x * x + y * y;
    }
    
    double dist(const Point& other) const {
        return sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
    }
};

// Cross product of vectors (p1-p0) and (p2-p0)
double cross(const Point& p0, const Point& p1, const Point& p2) {
    return (p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y);
}

// Build convex hull using Graham scan
std::vector<Point> convexHull(std::vector<Point> points) {
    int n = points.size();
    if (n < 3) return points;
    
    std::sort(points.begin(), points.end(), [](const Point& a, const Point& b) {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
    });
    
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

// Diameter of convex polygon using rotating calipers
double diameter(const std::vector<Point>& hull) {
    int n = hull.size();
    if (n < 2) return 0;
    if (n == 2) return hull[0].dist(hull[1]);
    
    double maxDist = 0;
    int j = 1;
    
    for (int i = 0; i < n; i++) {
        int nextI = (i + 1) % n;
        
        while (true) {
            int nextJ = (j + 1) % n;
            
            Point v1 = hull[nextI] - hull[i];
            Point v2 = hull[nextJ] - hull[j];
            
            if (v1.cross(v2) < 0) {
                j = nextJ;
            } else {
                break;
            }
        }
        
        double dist = hull[i].dist(hull[j]);
        maxDist = std::max(maxDist, dist);
    }
    
    return maxDist;
}

// Width of convex polygon (minimum distance between parallel supporting lines)
double width(const std::vector<Point>& hull) {
    int n = hull.size();
    if (n < 2) return 0;
    
    double minWidth = INT_MAX;
    int j = 1;
    
    for (int i = 0; i < n; i++) {
        int nextI = (i + 1) % n;
        Point edge = hull[nextI] - hull[i];
        double edgeLen = sqrt(edge.dist2());
        
        while (true) {
            int nextJ = (j + 1) % n;
            Point toNext = hull[nextJ] - hull[i];
            Point toCurr = hull[j] - hull[i];
            
            if (edge.cross(toNext) > edge.cross(toCurr)) {
                j = nextJ;
            } else {
                break;
            }
        }
        
        Point toJ = hull[j] - hull[i];
        double dist = abs(edge.cross(toJ)) / edgeLen;
        minWidth = std::min(minWidth, dist);
    }
    
    return minWidth;
}

// Minimum area bounding rectangle
double minAreaBoundingRect(const std::vector<Point>& hull) {
    int n = hull.size();
    if (n < 3) return 0;
    
    double minArea = INT_MAX;
    int j = 1, k = 1, l = 1;
    
    for (int i = 0; i < n; i++) {
        int nextI = (i + 1) % n;
        Point edge = hull[nextI] - hull[i];
        double edgeLen = sqrt(edge.dist2());
        
        while (edge.cross(hull[(j + 1) % n] - hull[i]) > 
               edge.cross(hull[j] - hull[i])) {
            j = (j + 1) % n;
        }
        
        while ((hull[(k + 1) % n] - hull[i]).dot(edge) > 
               (hull[k] - hull[i]).dot(edge)) {
            k = (k + 1) % n;
        }
        
        if (i == 0) l = j;
        
        while ((hull[(l + 1) % n] - hull[i]).dot(edge) < 
               (hull[l] - hull[i]).dot(edge)) {
            l = (l + 1) % n;
        }
        
        Point toJ = hull[j] - hull[i];
        double height = abs(edge.cross(toJ)) / edgeLen;
        
        Point toK = hull[k] - hull[i];
        Point toL = hull[l] - hull[i];
        double width = (toK.dot(edge) - toL.dot(edge)) / edgeLen;
        
        minArea = std::min(minArea, width * height);
    }
    
    return minArea;
}

// Example usage
int main() {
    std::vector<Point> points = {
        {0, 0}, {4, 0}, {4, 4}, {2, 6}, {0, 4}
    };
    
    std::vector<Point> hull = convexHull(points);
    
    std::cout << "Convex hull has " << hull.size() << " points" << std::endl;
    
    double diam = diameter(hull);
    std::cout << "Diameter: " << diam << std::endl;
    
    double w = width(hull);
    std::cout << "Width: " << w << std::endl;
    
    double minRect = minAreaBoundingRect(hull);
    std::cout << "Minimum bounding rectangle area: " << minRect << std::endl;
    
    return 0;
}

