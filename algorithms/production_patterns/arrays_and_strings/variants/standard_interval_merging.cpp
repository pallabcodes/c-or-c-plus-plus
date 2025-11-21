/*
 * Standard Interval Merging
 *
 * Source: Classic algorithmic problems, competitive programming
 * Algorithm: Sort intervals and merge overlapping/adjacent ones
 * Paper: Various algorithm textbooks and competitive programming resources
 *
 * What Makes It Ingenious:
 * - Sort and sweep: O(n log n) sorting, O(n) merging
 * - Handles all cases: overlapping, adjacent, nested intervals
 * - Optimal for one-time operations
 * - Simple and efficient implementation
 * - Used extensively in competitive programming and interviews
 * - Foundation for more complex interval algorithms
 *
 * When to Use:
 * - LeetCode-style interval problems
 * - Calendar event merging
 * - Resource allocation conflicts
 * - Meeting room scheduling
 * - Time slot optimization
 * - Range consolidation in general
 *
 * Real-World Usage:
 * - Calendar applications
 * - Resource scheduling systems
 * - Network range allocation
 * - Time-based conflict resolution
 * - Meeting scheduling software
 * - Resource management systems
 *
 * Time Complexity: O(n log n) due to sorting
 * Space Complexity: O(n) for output, O(1) extra space
 */

#include <vector>
#include <algorithm>
#include <iostream>
#include <stdexcept>

// Generic interval class
template<typename T = int>
struct Interval {
    T start;
    T end;  // Exclusive end for consistency

    Interval(T s = 0, T e = 0) : start(s), end(e) {
        if (start > end) {
            throw std::invalid_argument("Invalid interval: start > end");
        }
    }

    // Length of interval
    T length() const { return end - start; }

    // Check if intervals overlap
    bool overlaps(const Interval& other) const {
        return start < other.end && other.start < end;
    }

    // Check if intervals are adjacent (touching but not overlapping)
    bool adjacent(const Interval& other) const {
        return end == other.start || other.end == start;
    }

    // Check if this interval contains another
    bool contains(const Interval& other) const {
        return start <= other.start && other.end <= end;
    }

    // Merge two intervals (assumes they overlap or are adjacent)
    Interval merge(const Interval& other) const {
        return Interval(std::min(start, other.start),
                       std::max(end, other.end));
    }

    // Intersection of two intervals
    Interval intersection(const Interval& other) const {
        T new_start = std::max(start, other.start);
        T new_end = std::min(end, other.end);
        if (new_start >= new_end) {
            return Interval(0, 0); // Empty interval
        }
        return Interval(new_start, new_end);
    }

    // Check if interval is empty
    bool empty() const { return start >= end; }

    // Comparison for sorting
    bool operator<(const Interval& other) const {
        return start < other.start || (start == other.start && end < other.end);
    }

    void print() const {
        std::cout << "[" << start << ", " << end << ")";
    }
};

// Standard interval merging algorithm
template<typename T>
class IntervalMerger {
public:
    // Merge overlapping/adjacent intervals
    static std::vector<Interval<T>> merge_intervals(std::vector<Interval<T>> intervals) {
        if (intervals.empty()) {
            return {};
        }

        // Sort intervals by start time
        std::sort(intervals.begin(), intervals.end());

        std::vector<Interval<T>> merged;
        merged.push_back(intervals[0]);

        for (size_t i = 1; i < intervals.size(); ++i) {
            const auto& current = intervals[i];
            auto& last = merged.back();

            if (last.overlaps(current) || last.adjacent(current)) {
                // Merge with last interval
                last = last.merge(current);
            } else {
                // Add as new interval
                merged.push_back(current);
            }
        }

        return merged;
    }

    // Find intersection of multiple intervals
    static std::vector<Interval<T>> intersect_intervals(const std::vector<Interval<T>>& intervals) {
        if (intervals.empty()) {
            return {};
        }

        Interval<T> result = intervals[0];

        for (size_t i = 1; i < intervals.size(); ++i) {
            result = result.intersection(intervals[i]);
            if (result.empty()) {
                return {}; // No intersection
            }
        }

        return {result};
    }

    // Remove overlapping parts (find non-overlapping intervals)
    static std::vector<Interval<T>> subtract_intervals(
        const std::vector<Interval<T>>& base,
        const std::vector<Interval<T>>& subtract) {

        auto merged_base = merge_intervals(base);
        auto merged_subtract = merge_intervals(subtract);

        std::vector<Interval<T>> result;

        for (const auto& b : merged_base) {
            Interval<T> current = b;

            for (const auto& s : merged_subtract) {
                if (current.overlaps(s)) {
                    // Split current interval around the subtraction
                    if (current.start < s.start) {
                        result.push_back(Interval<T>(current.start, s.start));
                    }
                    if (s.end < current.end) {
                        current = Interval<T>(s.end, current.end);
                    } else {
                        current = Interval<T>(0, 0); // Mark as consumed
                        break;
                    }
                }
            }

            if (!current.empty()) {
                result.push_back(current);
            }
        }

        return result;
    }

    // Find gaps between intervals
    static std::vector<Interval<T>> find_gaps(
        const std::vector<Interval<T>>& intervals,
        T min_val, T max_val) {

        auto merged = merge_intervals(intervals);
        std::vector<Interval<T>> gaps;

        // Gap from min_val to first interval
        if (!merged.empty() && merged[0].start > min_val) {
            gaps.push_back(Interval<T>(min_val, merged[0].start));
        }

        // Gaps between intervals
        for (size_t i = 1; i < merged.size(); ++i) {
            if (merged[i-1].end < merged[i].start) {
                gaps.push_back(Interval<T>(merged[i-1].end, merged[i].start));
            }
        }

        // Gap from last interval to max_val
        if (!merged.empty() && merged.back().end < max_val) {
            gaps.push_back(Interval<T>(merged.back().end, max_val));
        }

        return gaps;
    }

    // Check if intervals cover a range completely
    static bool covers_range(const std::vector<Interval<T>>& intervals,
                           T start, T end) {
        auto merged = merge_intervals(intervals);

        if (merged.empty()) return false;

        // Check if first interval covers start
        if (merged[0].start > start) return false;

        // Check if last interval covers end
        if (merged.back().end < end) return false;

        // Check for gaps in between
        for (size_t i = 1; i < merged.size(); ++i) {
            if (merged[i-1].end < merged[i].start) {
                // There's a gap
                if (merged[i-1].end < end && merged[i].start > start) {
                    return false; // Gap in the range we care about
                }
            }
        }

        return true;
    }

    // Find all intersection points
    static std::vector<T> find_intersection_points(const std::vector<Interval<T>>& intervals) {
        std::vector<T> points;

        // Add all start and end points
        for (const auto& interval : intervals) {
            points.push_back(interval.start);
            points.push_back(interval.end);
        }

        // Sort and remove duplicates
        std::sort(points.begin(), points.end());
        auto last = std::unique(points.begin(), points.end());
        points.erase(last, points.end());

        return points;
    }
};

// Specialized for calendar/meeting scheduling
class MeetingScheduler {
private:
    using TimeInterval = Interval<int>; // Minutes since midnight

public:
    // Find available time slots
    static std::vector<TimeInterval> find_available_slots(
        const std::vector<TimeInterval>& meetings,
        int work_start, int work_end,
        int meeting_duration) {

        // Find gaps in the schedule
        auto gaps = IntervalMerger<int>::find_gaps(meetings, work_start, work_end);

        std::vector<TimeInterval> available_slots;

        for (const auto& gap : gaps) {
            // Check if gap is long enough for the meeting
            if (gap.length() >= meeting_duration) {
                available_slots.push_back(gap);
            }
        }

        return available_slots;
    }

    // Find conflicts between schedules
    static std::vector<TimeInterval> find_conflicts(
        const std::vector<TimeInterval>& schedule1,
        const std::vector<TimeInterval>& schedule2) {

        auto merged1 = IntervalMerger<int>::merge_intervals(schedule1);
        auto merged2 = IntervalMerger<int>::merge_intervals(schedule2);

        std::vector<TimeInterval> conflicts;

        for (const auto& i1 : merged1) {
            for (const auto& i2 : merged2) {
                if (i1.overlaps(i2)) {
                    auto intersection = i1.intersection(i2);
                    if (!intersection.empty()) {
                        conflicts.push_back(intersection);
                    }
                }
            }
        }

        return IntervalMerger<int>::merge_intervals(conflicts);
    }
};

// Example usage
int main() {
    std::cout << "Standard Interval Merging Demonstration:" << std::endl;

    // Basic interval merging
    std::vector<Interval<int>> intervals = {
        {1, 4}, {2, 6}, {8, 10}, {9, 12}, {15, 18}
    };

    std::cout << "Original intervals:" << std::endl;
    for (const auto& interval : intervals) {
        std::cout << "  ";
        interval.print();
        std::cout << std::endl;
    }

    auto merged = IntervalMerger<int>::merge_intervals(intervals);

    std::cout << "\nMerged intervals:" << std::endl;
    for (const auto& interval : merged) {
        std::cout << "  ";
        interval.print();
        std::cout << " (length: " << interval.length() << ")" << std::endl;
    }

    // Find gaps
    auto gaps = IntervalMerger<int>::find_gaps(intervals, 0, 20);
    std::cout << "\nGaps in range [0, 20):" << std::endl;
    for (const auto& gap : gaps) {
        std::cout << "  ";
        gap.print();
        std::cout << std::endl;
    }

    // Meeting scheduler example
    std::cout << "\nMeeting Scheduler Example:" << std::endl;

    std::vector<Interval<int>> meetings = {
        {9*60, 10*60},   // 9:00-10:00
        {11*60, 12*60},  // 11:00-12:00
        {14*60, 15*60}   // 2:00-3:00
    };

    int work_start = 8*60;  // 8:00
    int work_end = 17*60;   // 5:00
    int meeting_duration = 60; // 1 hour

    auto available = MeetingScheduler::find_available_slots(
        meetings, work_start, work_end, meeting_duration);

    std::cout << "Available 1-hour slots during work hours:" << std::endl;
    for (const auto& slot : available) {
        int start_hour = slot.start / 60;
        int start_min = slot.start % 60;
        int end_hour = slot.end / 60;
        int end_min = slot.end % 60;
        std::cout << "  " << start_hour << ":" << (start_min < 10 ? "0" : "")
                  << start_min << " - " << end_hour << ":" << (end_min < 10 ? "0" : "")
                  << end_min << std::endl;
    }

    return 0;
}

