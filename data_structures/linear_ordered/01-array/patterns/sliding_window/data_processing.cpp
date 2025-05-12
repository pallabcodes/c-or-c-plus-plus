// Real-Time Data Processing (Stream of Data with Sliding Window)

// Scenario : Tracking Recent Errors or Anomalies in Real - Time Logs
//                                                              For instance,
//     you’re designing a logging system that needs to keep track of error counts within the last 5 minutes(or the last k logs).Here, the sliding window isn’t about sum or max / min;
// it’s about dynamically adding new logs and removing old ones, while maintaining relevant statistics.

//                                                                   Hacky Approach : Instead of just tracking a simple moving average or
//                                                                   sum,
//     you can optimize how you handle event processing.Use multiple sliding windows with different bucket sizes(e.g., minute vs.second).Also, instead of updating logs naively, maintain pre - aggregated data in the sliding window and update it batch - wise when certain conditions(like time or threshold) are met.

#include <iostream>
#include <deque>
#include <vector>
#include <unordered_map>

class SlidingWindowLogger
{
public:
  SlidingWindowLogger(int windowSize) : windowSize(windowSize) {}

  void logEvent(int timestamp, const std::string &event)
  {
    // Clear events that are outside the current window
    while (!events.empty() && events.front().first <= timestamp - windowSize)
    {
      events.pop_front();
    }

    // Add the new event
    events.push_back({timestamp, event});
    eventCount[event]++;

    // Print events and counts as required by your system
    printRecentEvents();
  }

  void printRecentEvents()
  {
    std::cout << "Recent events in the last " << windowSize << " seconds:\n";
    for (auto &e : events)
    {
      std::cout << e.second << " at " << e.first << std::endl;
    }
    std::cout << "Event counts:\n";
    for (auto &count : eventCount)
    {
      std::cout << count.first << ": " << count.second << std::endl;
    }
  }

private:
  int windowSize;
  std::deque<std::pair<int, std::string>> events;  // Timestamp + event
  std::unordered_map<std::string, int> eventCount; // Event type count
};

int main()
{
  SlidingWindowLogger logger(10); // 10 seconds sliding window

  logger.logEvent(1, "error");
  logger.logEvent(2, "warning");
  logger.logEvent(5, "error");
  logger.logEvent(11, "error");

  return 0;
}

// Genius Hack:
// Sliding Window in Real-Time Systems: This is real-world use of sliding windows for processing streaming data or event logs, where you can dynamically maintain a window of recent events and process them efficiently. This goes beyond algorithmic problems to system design where sliding window helps in real-time processing.