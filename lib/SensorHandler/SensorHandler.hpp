#ifndef __SENSOR_HANDLER_HPP__
#define __SENSOR_HANDLER_HPP__

#include <Arduino.h>

class SensorHandler
{
private:
  time_t timestamp;

public:
  float tempC;
  float tempF;
  float humidity;
  float lpg;
  float co;
  float smoke;

  SensorHandler();
  void setup();
  void handle();
  void calibrate();
};

extern SensorHandler sensorHandler;

#endif
