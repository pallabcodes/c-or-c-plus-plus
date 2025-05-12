// Scenario : Real - Time Temperature Monitoring and Analysis
//                       Suppose you're monitoring temperature sensors in a large system (such as a smart city infrastructure). You may want to track the average temperature within the last k seconds, but you also want to keep track of different time windows dynamically (e.g., every 5 seconds vs. every minute).

//            Hacky Approach : Dynamic binning involves creating a sliding window that not only tracks recent data but also group data into separate bins based on time intervals,
//     with each window tracking distinct time slices in parallel.

// QUESTION: Dynamic Time-Windowed Analytics (Sliding Window with Timestamp Binning)

#include <iostream>
#include <deque>
#include <vector>
#include <numeric>

class DynamicTimeWindowAnalytics
{
public:
  DynamicTimeWindowAnalytics(int windowSize)
      : windowSize(windowSize) {}

  void addReading(int timestamp, int temperature)
  {
    // Remove expired windows
    while (!temperatureReadings.empty() && temperatureReadings.front().first <= timestamp - windowSize)
    {
      temperatureReadings.pop_front();
    }

    // Add the current reading
    temperatureReadings.push_back({timestamp, temperature});

    // Calculate the moving average
    calculateMovingAverage();
  }

  void calculateMovingAverage()
  {
    std::vector<int> temps;
    for (const auto &entry : temperatureReadings)
    {
      temps.push_back(entry.second);
    }
    double avg = std::accumulate(temps.begin(), temps.end(), 0.0) / temps.size();
    std::cout << "Average temperature in last " << windowSize << " seconds: " << avg << std::endl;
  }

private:
  int windowSize;
  std::deque<std::pair<int, int>> temperatureReadings; // (timestamp, temperature)
};

int main()
{
  DynamicTimeWindowAnalytics tempAnalyzer(10); // 10-second sliding window for average

  tempAnalyzer.addReading(1, 20);
  tempAnalyzer.addReading(3, 22);
  tempAnalyzer.addReading(6, 21);
  tempAnalyzer.addReading(10, 19);

  return 0;
}

// Genius Hack:
// Dynamic Sliding Windows with Bins: Here, the dynamic time windowing allows for multiple time-based windows (e.g., 10-second sliding windows), where readings can be grouped and tracked as they move forward. This system could be easily adapted to handle data streaming or real-time analytics in systems like IoT platforms.