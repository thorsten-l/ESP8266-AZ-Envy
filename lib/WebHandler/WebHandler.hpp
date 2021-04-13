#ifndef __WEB_HANDLER_H__
#define __WEB_HANDLER_H__

class WebHandler
{
private:
  bool initialized;
  void setup();

public:
  WebHandler();

  bool uploadIsRunning;
  void handle();
};

extern WebHandler webHandler;

#endif
