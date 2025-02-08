// #include <iostream>
// using namespace std;

int removeDuplicates(int arr[], int n) {
  if (n == 0)
    return 0;
  int slow = 0;

  for (int fast = 1; fast < n; fast++) {
    if (arr[fast] != arr[slow]) {
      slow++;
      arr[slow] = arr[fast];
    }
  }
  return slow + 1;
}
