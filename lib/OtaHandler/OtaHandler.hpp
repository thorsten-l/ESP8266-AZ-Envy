#ifndef __OTA_HANDLER_HPP__
#define __OTA_HANDLER_HPP__

class OtaHandler
{
private:
  bool initialized;
  void setup();

public:
  bool isRunning;
  void handle();
};

extern OtaHandler otaHandler;

#endif
