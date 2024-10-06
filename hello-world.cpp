// Just like Java, JavaScript and python has import, similary with C++ it is
// #include This particular library i.e. "iostream" is used for input and output
// streams and it will let us to use std::cout to print
#include <iostream>
#include <string> // Required for using std::string
#include <vector>

// What does the below line mean ? What does it tell to compiler ?
// 1. assume whatever property/method or object is gonna be used within this
// file, will come from this namespace i.e. "std" so if ever there is something
// to look for , look within "std namespace"

using namespace std;

class Student {
  string mFirst = "Jon";
  string mLast = "Doe";
  int mId = 1;
  float mAverage = 10.10;

  Student() {}

  Student(string first, string last, int id, float avg)
      : mFirst(first), mLast(last), mId(id), mAverage(avg) {}
};

// N.B: now, this below main function could be though of as similar psvm in
// java N.B: Each c++ program must have a main function which is run when the
// program starts argc = no of arguments, argv = array of arguments (similar to
// string args[] in java)
// N.B: on the "main" method both parameters are
int main(int argc, char *argv[]) {
  // std is a namespace that contains the cout output stream
  // << could be though of as whatever value at right of << will be send to the
  // namespace:method i.e. std::cout So, once again << takes the values from the
  // right and sends them to the output stream i.e. std::cout and std::count
  // knows how to handle or what to do with the value  i.e. print "Hello,
  // world!" to console N.B: << is a binary operator

  /**
   * 1. This statement outputs the string "Hello, world!" followed by a newline.
   * 2. The \n character explicitly creates a new line in the output.
   */
  cout << "Hello, world!\n";

  /**
   * 1.This statement outputs the string "Hello, World!" and then flushes the
   * output buffer, ensuring that the text is displayed immediately.
   * 2. std::endl also creates a new line after the output.
   */

  /**
   *
   * Both creates a newline: what is the difference ? \n vs std::endl ?
   *
   * No problem! Let’s break down what "flushing" means in simple terms.

### What is Flushing?

Flushing refers to the process of clearing or emptying the output buffer, which
is a temporary storage area for data before it gets displayed on the screen.

### How Output Works

1. **Output Buffer**: When you use `std::cout`, the text you want to display
isn't shown on the screen immediately. Instead, it goes into a temporary storage
area called the "output buffer."

2. **Why Use a Buffer?**: This helps the program run more efficiently. Instead
of displaying every single piece of text right away, it waits until there's a
good amount of text to show, or until it decides it’s time to show what’s in the
buffer.

3. **Flushing the Buffer**:
   - When you use `std::endl`, it tells the program, “Hey, show everything in
the buffer right now!” This clears the buffer and displays the text on the
screen immediately.
   - With `\n`, the program doesn’t force the buffer to clear right away. It
just adds a new line. The output might still be in the buffer until the program
is done or the buffer is full.

### Simple Analogy

Think of the output buffer like a **mailbox**:

- **Adding Mail**: When you send a letter (print text), it goes into the mailbox
(buffer).
- **Flushing with `std::endl`**: If you go to the mailbox and take out all the
letters to deliver (flush the buffer), everything gets delivered to the
recipient immediately.
- **Using `\n`**: If you just put a letter in the mailbox and don’t take any
letters out yet, it stays there until you decide to empty the mailbox.

   ### Summary

   * - **`std::endl`**: Flushing the buffer means showing everything inside it
right away.

   - **`\n`**: Just adds a new line but doesn’t force everything to be shown
immediately. So, using `\n` is like reading the letter (i.e. performing the
required action) from the mailbox but leaving it inside without delivering it.


   * #### Why std:endl a bit slower than \n ?
   * `std::endl` flushes the buffer first and then displays the message. So, it
does the following in this order:
   * 1. **Flush the Buffer**: It clears any content in the output buffer, making
sure everything is displayed on the screen.
   * 2. **Display the Message**: Then it shows the message you just printed.
   * This extra step of flushing is why `std::endl` can be a bit slower compared
to just using `\n`, which only adds a new line without forcing the output to
show immediately.
   */

  int a = 10;
  int b = 20;

  // std::cout.pipe(a).pipe(" ").pipe(b).pipe(std::endl)
  cout << a << " " << b << std::endl;

  // String
  string empty;
  string name = "John";
  string surname = "Wick";
  cout << name << std::endl;
  string fullName = name + " " + surname;
  cout << fullName << endl;

  // vector

  vector<int> vec; // initialized as an empty vector
  vec.push_back(42);
  vec.push_back(10);
  vec.push_back(11);

  // here, i (signed integer), vec.size (unsigned integer)
  /**
   * so, for safety use "size_t" so that on 32-bit system it will 32bits and
   * 64bits system it will 64-bits
   *
   * vec.size is unsigned where iterator i.e. i signed
   * so, it is best to use size_t for safety
   *
   * */

  for (size_t i = 0; i < vec.size(); i++) {
    cout << vec[i] << "\n";
  }

  // whatever element's type vec, infer it automatically
  for (auto a : vec) {
    cout << a << "\n";
  }

  // cout << "First element: " << vec[0] << endl;
  // cout << "Second element: " << vec[1] << endl;

  // size of vector
  cout << "Size of vector: " << vec.size() << endl;

  // Consider using vec.at() for safer element access
  // cout << "First element: " << vec.at(0) << endl;
  // cout << "Second element: " << vec.at(1) << endl;

  return 0;
}
