// Scenario : API Rate Limiting with Sliding Window
//                This is a typical use case in distributed
//                systems(like API gateways, web servers, etc.) where requests must be tracked and rate
//            -
//            limited based on time.

//            Hacky Approach:
// Instead of using a fixed - size sliding window, you can use a bucket - based sliding window, where each bucket represents a time slice.The trick here is to have multiple sliding windows running at once, where each window corresponds to a fixed time frame(e.g., 1 second, 1 minute, etc.), and they slide independently.

// Efficient Rate-Limiting (Sliding Window with Time Bins)

#include <iostream>
#include <deque>
#include <unordered_map>

class SlidingWindowRateLimiter
{
public:
  SlidingWindowRateLimiter(int windowSize, int maxRequests)
      : windowSize(windowSize), maxRequests(maxRequests) {}

  bool allowRequest(int timestamp, const std::string &userId)
  {
    // Remove requests outside the window
    while (!requests[userId].empty() && requests[userId].front() <= timestamp - windowSize)
    {
      requests[userId].pop_front();
    }

    // Check if we can allow the request
    if (requests[userId].size() < maxRequests)
    {
      // Add the new request timestamp
      requests[userId].push_back(timestamp);
      return true; // Allow request
    }
    else
    {
      return false; // Deny request
    }
  }

private:
  int windowSize;
  int maxRequests;
  std::unordered_map<std::string, std::deque<int>> requests; // User requests by timestamp
};

int main()
{
  SlidingWindowRateLimiter rateLimiter(60, 5); // 5 requests per 60 seconds per user

  // Simulate requests
  std::cout << "Request 1: " << rateLimiter.allowRequest(1, "user1") << std::endl;
  std::cout << "Request 2: " << rateLimiter.allowRequest(2, "user1") << std::endl;
  std::cout << "Request 6: " << rateLimiter.allowRequest(6, "user1") << std::endl;
  std::cout << "Request 10: " << rateLimiter.allowRequest(10, "user1") << std::endl;
  std::cout << "Request 60: " << rateLimiter.allowRequest(60, "user1") << std::endl;
  std::cout << "Request 61: " << rateLimiter.allowRequest(61, "user1") << std::endl; // Exceeds limit

  return 0;
}

// Genius Hack:
// Sliding Window for Rate Limiting: This uses the sliding window mechanism to enforce rate limiting, ensuring that the user cannot exceed the request limit in a given time frame, but instead of just counting requests, it dynamically updates the allowed count as time passes. This implementation takes advantage of time-based sliding windows.