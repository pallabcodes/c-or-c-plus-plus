#include <iostream>
#include <mutex>

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

  std::cout << worker1->getPrinterStatus() << std::endl;
  std::cout << worker2->getPrinterStatus() << std::endl;

  return 0;
}