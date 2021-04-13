#ifndef __SHT3X_HANDLER_H__
#define __SHT3X_HANDLER_H__

#include <Arduino.h>

#define UPDATE_INTERVAL 5000l

#define STATE_SEND_COMMAND 0
#define STATE_READ_DATA 1

class SHT3xHandler
{
private:
  bool initialized;
  uint8_t i2cAddress;
  time_t intervalTimestamp;
  uint8_t data[6];
  int state;

  int updateSHT3xValues();

public:
  float cTemp=0.0f;
  float fTemp=0.0f;
  float humidity=0.0f;
  time_t lastValidDataTimestamp;

  SHT3xHandler(uint8_t _address=0x45);
  void handle();
};

extern SHT3xHandler sht3xHandler;

#endif
