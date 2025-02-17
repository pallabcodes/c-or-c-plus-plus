#include <functional>
#include <iostream>
#include <vector>

using namespace std;

// Rule-Based Pattern Matching
class RuleBasedMatcher {
private:
  struct Rule {
    string pattern;
    function<bool(const string &)> predicate;
  };

  vector<Rule> rules;

public:
  void addRule(const string &pattern, function<bool(const string &)> pred) {
    rules.push_back({pattern, pred});
  }

  vector<string> match(const vector<string> &inputs) {
    vector<string> matches;
    for (const auto &input : inputs) {
      for (const auto &rule : rules) {
        if (rule.predicate(input)) {
          matches.push_back(input);
          break;
        }
      }
    }
    return matches;
  }
};

int main() {
  // Rule-Based Pattern Matcher
  RuleBasedMatcher matcher;

  // Add rules
  matcher.addRule("even", [](const string &s) { return stoi(s) % 2 == 0; });
  matcher.addRule("odd", [](const string &s) { return stoi(s) % 2 != 0; });
  matcher.addRule("length-2", [](const string &s) { return s.length() == 2; });

  // Sample input
  vector<string> inputs = {"12", "7", "23", "44", "5", "100"};

  // Match rules
  vector<string> matched = matcher.match(inputs);

  // Print results
  cout << "Matched values: ";
  for (const auto &s : matched) {
    cout << s << " ";
  }
  cout << endl;

  return 0;
}
