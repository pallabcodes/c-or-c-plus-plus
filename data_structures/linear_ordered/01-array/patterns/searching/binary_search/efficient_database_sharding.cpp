#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

bool canDistributeRecords(const vector<int> &records, int maxShards, int maxRecordsPerShard)
{
  int currentShardRecords = 0;
  int shardCount = 1; // Start with the first shard

  for (int record : records)
  {
    if (currentShardRecords + record > maxRecordsPerShard)
    {
      shardCount++;
      currentShardRecords = record;
      if (shardCount > maxShards)
        return false; // Too many shards needed
    }
    else
    {
      currentShardRecords += record;
    }
  }
  return true;
}

int findOptimalShardSize(const vector<int> &records, int maxShards)
{
  int left = *max_element(records.begin(), records.end());   // Min shard size
  int right = accumulate(records.begin(), records.end(), 0); // Max shard size

  while (left < right)
  {
    int mid = left + (right - left) / 2;
    if (canDistributeRecords(records, maxShards, mid))
    {
      right = mid; // Try reducing the shard size
    }
    else
    {
      left = mid + 1; // Increase the shard size
    }
  }
  return left;
}

int main()
{
  vector<int> records = {100, 200, 150, 250, 300}; // Simulated record sizes per shard
  int maxShards = 3;                               // Maximum allowed shards
  cout << "Optimal shard size: " << findOptimalShardSize(records, maxShards) << endl;
  return 0;
}
