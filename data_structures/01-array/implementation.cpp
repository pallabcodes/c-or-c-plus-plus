#include <iostream>
#include <stdexcept> // For std::out_of_range exception

using namespace std;

// Custom Array class with both fixed and dynamic size handling
template <typename T>

class MyArray
{
private:
  T *data;         // Pointer to dynamically allocated memory
  size_t size;     // Number of elements in the array
  size_t capacity; // Total capacity of the array

public:
  // Constructor for fixed-size array
  Array(size_t fixed_size)
  {
    size = fixed_size;
    capacity = fixed_size;
    data = new T[capacity]; // Dynamically allocate memory based on fixed size
  }

  // Constructor for dynamic array initialization (using initializer list)
  Array(initializer_list<T> list)
  {
    size = list.size();
    capacity = list.size();
    data = new T[capacity];               // Dynamically allocate memory
    copy(list.begin(), list.end(), data); // Copy data from initializer list
  }

  // Destructor to free dynamically allocated memory
  ~Array()
  {
    delete[] data; // Free dynamically allocated memory when the object is destroyed
  }

  // Function to get the size of the array
  size_t getSize() const
  {
    return size;
  }

  // Function to get the capacity of the array
  size_t getCapacity() const
  {
    return capacity;
  }
  // Function to add an element at the end of the array (resize if needed)
  void addElement(const T &element)
  {
    if (size == capacity)
    {                       // If the array is full, resize it
      resize(capacity * 2); // Double the capacity
    }
    data[size++] = element; // Add the new element and increment size
  }

  // Function to get an element at a given index
  T &operator[](size_t index)
  {
    if (index >= size)
    {
      throw out_of_range("Index out of range!"); // Check for out-of-range access
    }
    return data[index]; // Return the element at the specified index
  }

  // Const version of operator[] for read-only access
  const T &operator[](size_t index) const
  {
    if (index >= size)
    {
      throw out_of_range("Index out of range!"); // Check for out-of-range access
    }
    return data[index]; // Return the element at the specified index
  }

  // Function to resize the array
  void resize(size_t new_capacity)
  {
    if (new_capacity <= capacity)
    {
      return; // If the new capacity is smaller or equal, no resizing is needed
    }

    T *new_data = new T[new_capacity]; // Allocate new memory with the new capacity
    for (size_t i = 0; i < size; ++i)
    {
      new_data[i] = data[i]; // Copy old data to the new array
    }

    delete[] data;           // Free the old array memory
    data = new_data;         // Point to the new array
    capacity = new_capacity; // Update the capacity to the new value
  }

  // Function to print the elements of the array
  void print() const
  {
    cout << "Array: ";
    for (size_t i = 0; i < size; ++i)
    {
      cout << data[i] << " "; // Print each element followed by a space
    }
    cout << endl;
  }

  // Function to remove the last element of the array
  void removeLastElement()
  {
    if (size == 0)
    {
      cout << "Array is empty. Nothing to remove." << endl;
      return; // If the array is empty, nothing to remove
    }
    --size; // Decrement the size to "remove" the last element
  }
};

int main()
{
  // Fixed-size array initialization
  Array<int> fixedArray(5); // Fixed-size array with capacity 5

  // Add elements to the fixed-size array
  fixedArray.addElement(10);
  fixedArray.addElement(20);
  fixedArray.addElement(30);
  fixedArray.addElement(40);
  fixedArray.addElement(50);

  cout << "Fixed Array: ";
  fixedArray.print(); // Print the fixed-size array

  // Dynamic-size array using an initializer list
  Array<int> dynamicArray = {1, 2, 3, 4, 5};
  cout << "Dynamic Array: ";
  dynamicArray.print(); // Print the dynamic-size array

  // Accessing elements using the operator[]
  cout << "Element at index 2 in dynamic array: " << dynamicArray[2] << endl;

  // Resize the dynamic array
  dynamicArray.addElement(6); // Add one more element, array will resize
  dynamicArray.addElement(7);

  cout << "Resized Dynamic Array: ";
  dynamicArray.print(); // Print the resized dynamic array

  // Remove last element
  dynamicArray.removeLastElement();
  cout << "After removing last element: ";
  dynamicArray.print();

  return 0;
}

/**
 * Fixed Array: 10 20 30 40 50
 * Dynamic Array: 1 2 3 4 5
 * Element at index 2 in dynamic array: 3
 * Resized Dynamic Array: 1 2 3 4 5 6 7
 * After removing last element: 1 2 3 4 5 6
 */
