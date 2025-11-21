// Closest Pair of Points: Find two points with minimum distance
// Uses divide and conquer approach
// Time: O(n log^2 n) or O(n log n) with optimization
// Space: O(n)

#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <climits>
#include <limits>

using namespace std;

struct Point {
    int x, y;
    
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    
    double dist(const Point& other) const {
        return sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
    }
    
    bool operator<(const Point& other) const {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
};

double bruteForce(const vector<Point>& points, int left, int right) {
    double minDist = numeric_limits<double>::max();
    
    for (int i = left; i < right; i++) {
        for (int j = i + 1; j < right; j++) {
            minDist = min(minDist, points[i].dist(points[j]));
        }
    }
    
    return minDist;
}

double stripClosest(vector<Point>& strip, double d) {
    double minDist = d;
    sort(strip.begin(), strip.end(), [](const Point& a, const Point& b) {
        return a.y < b.y;
    });
    
    for (size_t i = 0; i < strip.size(); i++) {
        for (size_t j = i + 1; j < strip.size() && 
             (strip[j].y - strip[i].y) < minDist; j++) {
            minDist = min(minDist, strip[i].dist(strip[j]));
        }
    }
    
    return minDist;
}

double closestPairUtil(vector<Point>& points, int left, int right) {
    if (right - left <= 3) {
        return bruteForce(points, left, right);
    }
    
    int mid = (left + right) / 2;
    Point midPoint = points[mid];
    
    double dl = closestPairUtil(points, left, mid);
    double dr = closestPairUtil(points, mid, right);
    double d = min(dl, dr);
    
    vector<Point> strip;
    for (int i = left; i < right; i++) {
        if (abs(points[i].x - midPoint.x) < d) {
            strip.push_back(points[i]);
        }
    }
    
    return min(d, stripClosest(strip, d));
}

double closestPair(vector<Point> points) {
    sort(points.begin(), points.end());
    return closestPairUtil(points, 0, points.size());
}

// Example usage
int main() {
    vector<Point> points = {
        {2, 3}, {12, 30}, {40, 50}, {5, 1}, {12, 10}, {3, 4}
    };
    
    double minDist = closestPair(points);
    
    cout << "Closest pair distance: " << minDist << endl;
    
    return 0;
}

