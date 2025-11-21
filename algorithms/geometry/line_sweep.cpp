// Line Sweep Algorithm: Process geometric events in sorted order
// Useful for intersection detection, closest pair, etc.
// Time: O(n log n) typically
// Space: O(n)

#include <vector>
#include <set>
#include <algorithm>
#include <iostream>
#include <cmath>

using namespace std;

struct Point {
    int x, y;
    
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    
    bool operator<(const Point& other) const {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
};

struct Segment {
    Point p1, p2;
    int id;
    
    Segment(Point p1, Point p2, int id) : p1(p1), p2(p2), id(id) {}
    
    bool operator<(const Segment& other) const {
        // Compare by y-coordinate at current x
        return p1.y < other.p1.y;
    }
};

// Check if two segments intersect
bool segmentsIntersect(const Segment& s1, const Segment& s2) {
    auto orientation = [](const Point& p, const Point& q, const Point& r) {
        int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
        if (val == 0) return 0;
        return (val > 0) ? 1 : 2;
    };
    
    auto onSegment = [](const Point& p, const Point& q, const Point& r) {
        return q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
               q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y);
    };
    
    int o1 = orientation(s1.p1, s1.p2, s2.p1);
    int o2 = orientation(s1.p1, s1.p2, s2.p2);
    int o3 = orientation(s2.p1, s2.p2, s1.p1);
    int o4 = orientation(s2.p1, s2.p2, s1.p2);
    
    if (o1 != o2 && o3 != o4) return true;
    
    if (o1 == 0 && onSegment(s1.p1, s2.p1, s1.p2)) return true;
    if (o2 == 0 && onSegment(s1.p1, s2.p2, s1.p2)) return true;
    if (o3 == 0 && onSegment(s2.p1, s1.p1, s2.p2)) return true;
    if (o4 == 0 && onSegment(s2.p1, s1.p2, s2.p2)) return true;
    
    return false;
}

// Find all intersecting segments using line sweep
vector<pair<int, int>> findIntersections(const vector<Segment>& segments) {
    vector<pair<int, int>> intersections;
    
    // Create events: segment start and end
    vector<pair<Point, int>> events; // point and segment id
    for (const auto& seg : segments) {
        events.push_back({seg.p1, seg.id});
        events.push_back({seg.p2, seg.id});
    }
    
    sort(events.begin(), events.end());
    
    set<Segment> activeSegments;
    
    for (const auto& event : events) {
        int segId = event.second;
        Segment seg = segments[segId];
        
        // Check intersections with active segments
        auto it = activeSegments.lower_bound(seg);
        
        if (it != activeSegments.end() && segmentsIntersect(seg, *it)) {
            intersections.push_back({seg.id, it->id});
        }
        
        auto prev = it;
        if (prev != activeSegments.begin()) {
            prev--;
            if (segmentsIntersect(seg, *prev)) {
                intersections.push_back({seg.id, prev->id});
            }
        }
        
        // Add or remove segment
        if (activeSegments.find(seg) != activeSegments.end()) {
            activeSegments.erase(seg);
        } else {
            activeSegments.insert(seg);
        }
    }
    
    return intersections;
}

// Example usage
int main() {
    vector<Segment> segments = {
        {{1, 1}, {4, 4}, 0},
        {{2, 3}, {5, 1}, 1},
        {{3, 2}, {6, 5}, 2}
    };
    
    vector<pair<int, int>> intersections = findIntersections(segments);
    
    cout << "Intersecting segments:" << endl;
    for (auto [id1, id2] : intersections) {
        cout << "Segment " << id1 << " intersects with segment " << id2 << endl;
    }
    
    return 0;
}

