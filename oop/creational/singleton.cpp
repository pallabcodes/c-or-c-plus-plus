#include <iostream>
#include <mutex>

using namespace std;



class PrinterService
{
private:
  static PrinterService *uniqueInstance;
  static std::mutex mtx;

  std::string mode;

  PrinterService() : mode("GrayScale") {}

public:
  PrinterService(const PrinterService &) = delete;
  PrinterService &operator=(const PrinterService &) = delete;

  static PrinterService *getInstance()
  {
    if (uniqueInstance == nullptr)
    {
      std::lock_guard<std::mutex> lock(mtx);
      if (uniqueInstance == nullptr)
      {
        uniqueInstance = new PrinterService();
      }
    }
    return uniqueInstance;
  }

  std::string getPrinterStatus()
  {
    return mode;
  }

  void setMode(const std::string &newMode)
  {
    mode = newMode;
    std::cout << "Mode changed to " << mode << std::endl;
  }
};

PrinterService *PrinterService::uniqueInstance = nullptr;
std::mutex PrinterService::mtx;

int main()
{
  PrinterService *worker1 = PrinterService::getInstance();
  PrinterService *worker2 = PrinterService::getInstance();

  worker1->setMode("Color");
  worker2->setMode("Grayscale");

  cout << worker1->getPrinterStatus() << endl;
  cout << worker2->getPrinterStatus() << endl;

  return 0;
}

// -- -- -- -- -- -- -- -- -- -- -- --THREAD SAFE LAZY SINGLETON-- -- -- -- -- -- -- --

// #include <iostream>
// #include <mutex>

// class LazySingleton
// {
// private:
//   // The single instance, initially null
//   static LazySingleton *instance;
//   static std::mutex mutex;

//   // Private constructor to prevent instantiation
//   LazySingleton() {}

// public:
//   // Public method to get the instance
//   static LazySingleton *getInstance()
//   {
//     std::lock_guard<std::mutex> lock(mutex); // Ensures thread safety
//     // Check if instance is null
//     if (instance == nullptr)
//     {
//       // If null, create a new instance
//       instance = new LazySingleton();
//     }
//     // Return the instance (either newly created or existing)
//     return instance;
//   }

//   // Delete copy constructor and assignment operator to prevent copying
//   LazySingleton(const LazySingleton &) = delete;
//   LazySingleton &operator=(const LazySingleton &) = delete;

//   // For demonstration purposes
//   void showMessage()
//   {
//     std::cout << "Singleton instance accessed!\n";
//   }
// };

// // Initialize static members
// LazySingleton *LazySingleton::instance = nullptr;
// std::mutex LazySingleton::mutex;

// int main()
// {
//   // Access the singleton instance
//   LazySingleton *singleton = LazySingleton::getInstance();
//   singleton->showMessage();

//   return 0;
// }
