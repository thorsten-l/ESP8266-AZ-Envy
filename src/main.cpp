#include <Arduino.h>
#include <App.hpp>
#include <MqttHandler.hpp>
#include <OtaHandler.hpp>
#include <WebHandler.hpp>
#include <WifiHandler.hpp>
#include <SensorHandler.hpp>
#include <TelnetStream.h>
#include <uzlib.h>

unsigned long lifeTicker;
unsigned long maxLoopTime;
unsigned long lastLoopTimestamp;
unsigned long thisLoopTimestamp;
volatile unsigned long debounceTimestamp;
volatile unsigned long buttonTimestamp;
volatile bool buttonPressed;
volatile bool lastButtonState;

#define DEBOUNCE_TIME 500

void ICACHE_RAM_ATTR powerButtonPressed()
{
  if ((millis() - debounceTimestamp > DEBOUNCE_TIME))
  {
    buttonPressed = true;
    LOG1( "Button pressed %d\n", digitalRead(POWER_BUTTON));
  }

  debounceTimestamp = millis();
}

void setup()
{
  buttonPressed = false;
  app.setup();
  app.writeConfig();
  app.printConfig(appcfg);
  sensorHandler.setup();
  wifiHandler.setup();

  attachInterrupt(digitalPinToInterrupt(POWER_BUTTON), &powerButtonPressed,
                  FALLING);

  maxLoopTime = 0l;
  lifeTicker = lastLoopTimestamp = millis();
  uzlib_init();
}

void loop()
{
  thisLoopTimestamp = millis();
  maxLoopTime = max(maxLoopTime, thisLoopTimestamp - lastLoopTimestamp);
  lastLoopTimestamp = thisLoopTimestamp;

  if (buttonPressed)
  {
    tlog("button pressed\n");
    sensorHandler.calibrate();
    buttonPressed = false;
  }

  if ((thisLoopTimestamp - lifeTicker) >= 10000)
  {
    LOG1("wifi is connected %d\n", wifiHandler.isConnected());

    tlogf("max loop time = %ld\n", maxLoopTime);
    tlogf("free heap %d\n", ESP.getFreeHeap());

    maxLoopTime = 0l;
    lifeTicker = thisLoopTimestamp;
  }

  if (wifiHandler.handle(thisLoopTimestamp))
  {
    otaHandler.handle();
    if ( otaHandler.isRunning == false )
    {
      TelnetStream.handle();
      webHandler.handle();
      if ( webHandler.uploadIsRunning == false )
      {
        sensorHandler.handle();
        mqttHandler.handle();
      }
    }
  }

  app.handle(thisLoopTimestamp);

  delay(20); // time for IP stack
}
