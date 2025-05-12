#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>

using namespace std;

/**
 * 1. Dynamic Load Balancing in Distributed Systems
 * In a distributed system, you might have multiple servers or machines, each with
 * different load conditions. The task is to assign new jobs to servers while
 * ensuring that no server exceeds a certain threshold load. This is a real-world
 * case where binary search on the maximum load becomes key.
 *
 * Scenario:
 * You want to dynamically balance load across machines in a cluster, ensuring that
 * the load on any machine doesn't exceed a certain threshold, but you also want to
 * minimize the maximum load across the system.
 *
 */

bool canBalance(const vector<int> &jobs, int threshold, int m)
{
  int current_sum = 0;
  int machine_count = 1; // At least one machine

  for (int job : jobs)
  {
    if (current_sum + job > threshold)
    {
      machine_count++; // Need a new machine
      current_sum = job;
      if (machine_count > m)
        return false; // Too many machines
    }
    else
    {
      current_sum += job;
    }
  }
  return true;
}

int findOptimalLoadBalance(vector<int> &jobs, int m)
{
  int left = *max_element(jobs.begin(), jobs.end());   // Min threshold is the largest job
  int right = accumulate(jobs.begin(), jobs.end(), 0); // Max threshold is the sum of all jobs

  while (left < right)
  {
    int mid = left + (right - left) / 2;
    if (canBalance(jobs, mid, m))
    {
      right = mid; // Try a smaller threshold
    }
    else
    {
      left = mid + 1; // Increase the threshold
    }
  }
  return left; // Optimal threshold found
}

int main()
{
  vector<int> jobs = {10, 20, 30, 40, 50};
  int m = 3; // 3 machines
  cout << "Optimal load balance: " << findOptimalLoadBalance(jobs, m) << endl;
  return 0;
}

// Why Binary Search?

// Binary search here is applied on the possible maximum load that a machine can carry.Instead of brute - forcing the solution by trying every possible threshold and simulating the job distribution, binary search efficiently narrows down the feasible maximum load.This is critical when you're balancing loads dynamically across a large cluster in a system that demands real-time adjustments.