#include <bits/stdc++.h>
using namespace std;

// TODO: fix the syntax error

// N.B: c++ stl is divided into four parts : container (required), iterator (required), algorithm (required), functions (not required)

// container pre-requisite : Pair
void explainPair()
{
    // Basic pair
    pair<int, int> p1 = {1, 3};
    cout << "Basic pair: " << p1.first << " " << p1.second << endl;

    // Nested pair
    pair<int, pair<int, int>> p2 = {1, {3, 4}};
    cout << "Nested pair: " << p2.first << " " << p2.second.first << " " << p2.second.second << endl;

    // Array of pairs and with array of pairs here it has three elements so their indexes are 0, 1 and 2
    pair<int, int> arr[] = {{1, 2}, {3, 4}, {5, 6}};
    cout << "Array of pairs, first index: second element: " << arr[1].second << endl;
}

// container: Vector Examples (internally uses singly linked list)
void explainVector()
{
    // N.B: vector is similar to array but whereas array has a fixed size but vector could be dynamically sized

    // Different ways to initialize vectors
    vector<int> v1;     // creates the empty container e.g. {}
    v1.push_back(1);    // Add elements e.g. {1}
    v1.emplace_back(2); // it dynamically increased the size needed and then push at the back i.e. {1, 2} so it needed the space to hold another integer so it adds that space dynamically and then add the value at the back i.e. {1, 2} and "emplace_back is faster than push_back, read more on this"

    // Vector of pairs
    vector<pair<int, int>> vec;
    vec.push_back({1, 2});  // to add pair used curly braces
    vec.emplace_back(3, 4); // does the same i.e adds pair without using curly braces

    // Initialize vector "with initial size and default value" and vector could always increase in size dynamically later as needed
    // so, just because initialize size is given doesn't mean it will become fixed , vector is always dynamic while having the option to contain initial size
    vector<int> v2(5, 100); // size, value so it will be as : {100, 100, 100, 100, 100}
    // Copy vector
    vector<int> v3(v2); // Copy v2 to v3

    // Initialize vector with initial size but not explicit default value so it will either 0 or any garbage value (so let's assume these will be 0s)
    vector<int> v4(5); // {0, 0, 0, 0, 0}

    // Vector operations
    cout << "Vector size: " << v1.size() << endl;
    cout << "First element: " << v1.front() << endl;
    cout << "Last element: " << v1.back() << endl;

    // Iterator examples : vector<datatype>:: iterator iteratorName
    vector<int>::iterator it = v1.begin();                    // v.begin() points to the memory address (not the value itself at the said index i.e. at 0 by default)
    cout << "First element using iterator: " << *it << endl;  // since, it is pointer iterator variable so to print actual value, used * so it prints actual value from the current index (i.e. 0) and the value is 1
    it++;                                                     // so, now it is incremented now it is at index 1
    cout << "Second element using iterator: " << *it << endl; // similarly, it should print 2

    vector<int>::iterator ite = v1.end(); // {1, 2} no it doesn't point at memory address of 2 rather points to memory address right after the last element so after 2 therefore to make it point correctly, decrement ite--
    vector<int>::iterator itre = v1.rend();
    vector<int>::iterator itrbe = v1.rbegin();

    cout << v[0] << " " << v.at[0] << endl; // {10, 20} v[0] = 10 or v.at[0] (at isn't that used much)
    cout << v.back() << "";                 // access the last element e.g. {10, 20} so it will access 20

    for (vector<int>::iterator it = v1.begin(); it != v1.end(); it++)
    {
        cout << *it << endl;
    }

    // with, "auto", type inferred implicitly from v1.begin (so, no need to vector<int>:: iterator it) or if don't know the data type just use "auto"
    for (auto it = v1.begin(); it != v1.end(); it++)
    {
        cout << *it << endl;
    }

    // # deletion

    // Erase operations e.g. {10, 20, 12, 23}
    v1.erase(v1.begin()); // Erase first element so vector will be reshuffled or updated and now {20, 12, 23}

    // Erase operations e.g. {10, 20, 12, 23, 35}
    v1.erase(v1.begin() + 2, v1.begin() + 4); // (start, end) so after deletion {10, 20, 35 }

    // # insert

    vector<int> add(2, 100);            // adds 2 values of integer 100 i.e. {100, 100}
    add.insert(add.begin(), 300);       // {300, 100, 100}
    add.insert(add.begin() + 1, 2, 10); // starting from at index 1, adds 10 no. of times (i.e. here 2) so it will look like {300, 10, 10, 100, 100}

    vector<int> copy(2, 50);                           // {50,50}
    add.insert(add.begin(), copy.begin(), copy.end()); // {50, 50, 300, 10, 10, 100, 100}

    // Clear the entire vector
    v1.clear();

    cout << "Vector empty after clear: " << v1.empty() << endl;
}

// container : list (same as vector but whereas vector only has back manipulation whether push_back or emplace_back list has front operations as well)
// N.B: since, list is just as vector (with the diff. of front operations) so of course "list is also dynamic in nature"
void explainList()
{
    list<int> ls;

    ls.push_back(2);    // {2}
    ls.emplace_back(4); // {2, 4}

    ls.push_front(5);   // {5, 2, 4} -> it is efficient since internally it uses doubly linked list
    ls.emplace_front(); // {2, 4}

    // rest functions are same as vector i.e. begin, end, rbegin, rend, clear, insert, size and swap
}

void explainDeque()
{
    deque<int> dq;
    dq.push_back(1);    // {1}
    dq.emplace_back(2); // {1, 2}

    dq.push_front(4);    // {4, 1, 2}
    dq.emplace_front(3); // {3, 4, 1, 2}

    dq.pop_back();  // {3, 4, 1}
    dq.pop_front(); // {4, 1}

    dq.back();  // 1
    dq.front(); // 4

    // rest functions are same as vector i.e. begin, end, rbegin, rend, clear, insert, size and swap
}

void explainStack()
{
    stack<int> st;
    st.push(1);    // {1}
    st.push(2);    // {2, 1} {top, bottom} therefore it is looking like this
    st.push(3);    // {3, 2, 1}
    st.push(3);    // {3, 3, 2, 1}
    st.emplace(5); // {5, 3, 3, 2, 1} once again top i.e. 5 and bottom i.e. 1

    cout << st.top() << endl; // so, here it prints 5

    st.pop(); //{3, 3, 2, 1}

    cout << st.top() << endl;  //  3
    cout << st.size() << endl; // 4

    cout << st.empty();

    stack<int> st1, st2;
    st1.swap(st2);

    // rest functions are same as vector i.e. begin, end, rbegin, rend, clear, insert, size and swap
}

// container
void explainQueue()
{
    queue<int> q;
    q.push(1);    // {1}
    q.push(2);    // {1, 2} {bottom, top} therefore it is looking like this
    q.emplace(4); // {1, 2, 4}

    cout << "FRONT: " << q.front() << endl; // 1
    cout << "BACK " << q.back() << endl;    // 4

    q.back() += 5 cout << "BACK " << q.back() << endl; // 9

    cout << q.pop() << endl; // so, as it follows fifo so it will remove 1 so queue will look like {2,9}

    cout << q.front() << endl; // 2

    cout << q.empty();

    queue<int> q1, q2;
    q1.swap(q2);

    // rest functions are same as vector i.e. begin, end, rbegin, rend, clear, insert, size and swap
}

// container
void explainPQ()
{
    priority_queue<int> pq;
    pq.push(5);     // {1}
    pq.push(2);     // {5, 2}
    pq.push(8);     // {8, 5, 2}
    pq.emplace(10); // {10, 8, 5, 2}

    cout << pq.top() << endl; // 10

    pq.pop(); // {8, 5, 2}

    cout << pq.top() << endl; // 8

    // size swap empty function same as others

    // # Minimum heap
    priority_queue<int, vector<int>, greater<int>> pq;
    pq.push(5);     // {1}
    pq.push(2);     // {2, 5}
    pq.push(8);     // {2, 5 ,8}
    pq.emplace(10); // {2, 5 ,8, 10}

    cout << pq.top() << endl; // 2
}

// container: set (it stores everything in "sorted manner" and only contain "unique" values)

void explainSet()
{
    set<int> st;
    st.insert(1);  // {1}
    st.emplace(2); // {1, 2} {bottom, top}
    st.insert(2);  // {1, 2}
    st.insert(4);  // {1, 2, 4}
    st.insert(3);  // {1, 2, 3, 4}

    // rest functions are same as vector i.e. begin, end, rbegin, rend, clear, insert, size and swap

    // {1, 2, 3, 4, 5} -> {1, 2, 3, 4}
    st.erase(5); // takes the logarithmic time

    int count = st.count(1); // if the value 1 exist in the SET then TRUE or 1 else FALSE or 0

    // {1, 2, 3, 4, 5}
    auto it = st.find(3); // it will return an iterator (i.e. pointer variable that points to the memory address of said index or initially at the 0) but here it will "find and return `memory address` of value 3"
    st.erase(it);         // it takes constant time

    // {1, 2, 3, 4, 5}
    auto it = st.find(6); // but value 6 is not here so the iterator will point to address after the last (here 5) that could some garbage address

    // {1, 2, 3, 4, 5}
    auto it1 = st.find(2);
    auto it2 = st.find(4);
    st.erase(it1, it2); // {1, 4, 5}
}

void explainMultiSet()
{
    // Everything is same as SET but it stores duplicate values as well so it is "SORTED but stores duplicate values"

    multiset<int> ms;
    ms.insert(1); // {1}
    ms.insert(1); // {1, 1}
    ms.insert(1); // {1, 1, 1}

    ms.erase(1); // now, this will delete all 1 values from ms

    int count = ms.count(1);

    // but to delete / erase single value
    ms.erase(ms.find(1)); // so, now instead of a value e.g. ms.erase(1) that will delete all 1s rather used the memory address of a value to delete that only

    // similarly, multiple delete will work same way
    ms.erase(ms.find(1), ms.find(1) + 2); // start = 0, end = 2 so index 0 and 1 will be deleted and ms will look like {1}

    // rest are same as SET
}

void explainUSet()
{
    // Everything is same as SET but just not SORTED so unique values but unsorted

    multiset<int> ms;
    ms.insert(1); // {1}
    ms.insert(1); // {1, 1}
    ms.insert(1); // {1, 1, 1}

    ms.erase(1); // now, this will delete all 1 values from ms

    int count = ms.count(1);

    // but to delete / erase single value
    ms.erase(ms.find(1)); // so, now instead of a value e.g. ms.erase(1) that will delete all 1s rather used the memory address of a value to delete that only

    // similarly, multiple delete will work same way
    ms.erase(ms.find(1), ms.find(1) + 2); // start = 0, end = 2 so index 0 and 1 will be deleted and ms will look like {1}

    // rest are same as SET
}

void explainMap()
{
    // Simple map of int to int
    map<int, int> mp1;

    // Map of int to pair
    map<int, pair<int, int>> mp2;

    // Map with pair as key
    map<pair<int, int>, int> mp3;

    // Different ways to insert into simple map
    mp1[1] = 2;         // Basic insertion
    mp1.emplace(3, 1);  // Using emplace
    mp1.insert({2, 4}); // Using insert

    // Inserting into map with pair as value
    mp2[1] = {2, 3};

    // Inserting into map with pair as key
    mp3[{2, 3}] = 10;

    // Printing simple map
    cout << "Simple map (int -> int):" << endl;
    for (const auto &it : mp1)
    {
        cout << it.first << " -> " << it.second << endl;
    }

    // Printing map with pair as value
    cout << "\nMap with pair value (int -> pair):" << endl;
    for (const auto &it : mp2)
    {
        cout << it.first << " -> {" << it.second.first << ", " << it.second.second << "}" << endl;
    }

    // Printing map with pair as key
    cout << "\nMap with pair key (pair -> int):" << endl;
    for (const auto &it : mp3)
    {
        cout << "{" << it.first.first << ", " << it.first.second << "} -> " << it.second << endl;
    }

    // Demonstrating map access
    cout << "\nAccessing elements:" << endl;
    cout << "mp1[1] = " << mp1[1] << endl; // Existing key
    cout << "mp1[5] = " << mp1[5] << endl; // Non-existing key (will create with default value 0)

    // Additional map operations
    cout << "\nMap operations:" << endl;
    cout << "Size of mp1: " << mp1.size() << endl;

    // Check if key exists
    if (mp1.find(2) != mp1.end())
    {
        cout << "Key 2 exists in mp1" << endl;
    }

    // Erase by key
    mp1.erase(1);
    cout << "After erasing key 1, size: " << mp1.size() << endl;

    // Clear the map
    mp1.clear();
    cout << "After clearing, size: " << mp1.size() << endl;
}

void explainMultiMap()
{
    // same as the map, so SORTED keys but just it can store multiple keys
    // only mpp[key] can't be used here
}

void explainUMap()
{
    // same as the map, but UNSORTED
}

void

// Additional utility functions
void printVector(vector<int> &v)
{
    cout << "Vector elements: ";
    for (int x : v)
        cout << x << " ";
    cout << endl;
}

int main()
{
    cout << "=== Pair Examples ===" << endl;
    explainPair();

    cout << "\n=== Vector Examples ===" << endl;
    explainVector();

    // Additional vector demonstrations
    vector<int> numbers = {1, 2, 3, 4, 5};
    printVector(numbers);

    // Insert at specific position
    numbers.insert(numbers.begin() + 2, 10);
    cout << "After inserting 10 at position 2: ";
    printVector(numbers);

    // Sort vector
    sort(numbers.begin(), numbers.end());
    cout << "After sorting: ";
    printVector(numbers);

    // Find element
    auto it = find(numbers.begin(), numbers.end(), 10);
    if (it != numbers.end())
        cout << "Found 10 at position: " << it - numbers.begin() << endl;

    return 0;
}