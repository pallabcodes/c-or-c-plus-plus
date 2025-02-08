#include <cctype>
#include <iostream>
using namespace std;

bool isPalindrome(string s) {
  int left = 0, right = s.size() - 1;
  while (left < right) {
    while (left < right && !isalnum(s[left]))
      left++; // Ignore non-alphanumeric
    while (left < right && !isalnum(s[right]))
      right--;

    if (tolower(s[left]) != tolower(s[right]))
      return false;
    left++, right--;
  }
  return true;
}

int main() {
  string s = "A man, a plan, a canal: Panama";
  cout << (isPalindrome(s) ? "Palindrome" : "Not a Palindrome") << endl;
  return 0;
}
