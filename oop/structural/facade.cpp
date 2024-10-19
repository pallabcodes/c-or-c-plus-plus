#include <iostream>

class SmartHomeSubSystem
{

public:
  enum class Brightness
  {
    UNKNOWN,
    BRIGHT,
    DIM
  };

  enum class Service
  {
    UNKNOWN,
    HULU,
    NETFLIX,
    HBO
  };

  SmartHomeSubSystem()
      : brightness(Brightness::UNKNOWN),
        temperature(19),
        isSecurityArmed(false),
        streamingService(Service::UNKNOWN) {}

  void setBrightness(Brightness brightness)
  {
    this->brightness = brightness;
  }

  void setTemperature(int temperature)
  {
    this->temperature = temperature;
  }

  void setIsSecurityArmed(bool isSecurityArmed)
  {
    this->isSecurityArmed = isSecurityArmed;
  }

  void setStreamingService(Service streamingService)
  {
    this->streamingService = streamingService;
  }

private:
  void enableMotionSensors()
  {
    // ...
  }

  void updateFirmware()
  {
    // ...
  }

  Brightness brightness;
  int temperature;
  bool isSecurityArmed;
  Service streamingService;
};

class SmartHomeFacade
{

public:
  SmartHomeFacade(SmartHomeSubSystem &smartHome) : smartHome(smartHome) {}

  void setMovieMode()
  {
    smartHome.setBrightness(SmartHomeSubSystem::Brightness::DIM);
    smartHome.setTemperature(21);
    smartHome.setIsSecurityArmed(false);
    smartHome.setStreamingService(SmartHomeSubSystem::Service::NETFLIX);
  }

  void setFocusMode()
  {
    smartHome.setBrightness(SmartHomeSubSystem::Brightness::BRIGHT);
    smartHome.setTemperature(22);
    smartHome.setIsSecurityArmed(true);
    smartHome.setStreamingService(SmartHomeSubSystem::Service::UNKNOWN);
  }

private:
  SmartHomeSubSystem &smartHome;
};

int main()
{
  SmartHomeSubSystem smartHome;
  SmartHomeFacade f(smartHome);
  f.setMovieMode();
  f.setFocusMode();

  return 0;
}