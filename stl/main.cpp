#include <bits/stdc++.h> // instead of bringing in indiduvaal libararies, just import all libraries
#include <math.h>

using namespace std;

// STL is divided into four parts : Algorithms, Containers, Functions,  Iterators

void explainPair()
{
  pair<int, int> p1 = {1, 3};
  cout << p1.first << " " << p1.second;

  pair<int, pair<int, int>> p2 = {1, {3, 4}};
  cout << p2.first << " " << p2.second.first << " " << p2.second.second;

  pair<int, int> arr[] = {{1, 2}, {3, 4}, {5, 6}, {7, 8}};
  cout << arr[1].second;
}

void print()
{
  cout << "raj";
}

int sum(int a, int b)
{
  return a + b;
}

int main()
{
  print();
  int s = sum(2, 4);
  cout << s;

  vector<int> v;
  v.push_back(1);
  v.emplace_back(2);

  vector<pair<int, int>> vec;
  vec.push_back({10, 20});
  vec.emplace_back(10, 20);

  vector<int> digitFiveTo100(5, 100);
  vector<int> digitFive(5);

  vector<int> v1(5, 20);
  vector<int> v2(v1);

  vector<int>::iterator it = v.begin();
  it++;
  cout << *(it) << " ";

  return 0;
}
