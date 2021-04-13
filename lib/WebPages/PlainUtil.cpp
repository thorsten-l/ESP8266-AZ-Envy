#include <App.hpp>
#include <WifiHandler.hpp>
#include <ESP8266WebServer.h>
#include <SensorHandler.hpp>
#include <Util.hpp>
#include "WebPages.h"

extern ESP8266WebServer server;

void handlePlainFloatValue(float value)
{
  sprintf( buffer, "%.8f", value );
  server.sendHeader("Access-Control-Allow-Origin", "*");
  sendHeaderNoCache();
  server.send(200, "text/plain", buffer);
}

void handlePlainStatus()
{
  sprintf( buffer, 
    "temp_c=%.02f\n"
    "temp_f=%.02f\n"
    "humidity=%.01f\n"
    "lpg=%.08f\n"
    "co=%.08f\n"
    "smoke=%.08f\n",
    sensorHandler.tempC,
    sensorHandler.tempF,
    sensorHandler.humidity,
    sensorHandler.lpg,
    sensorHandler.co,
    sensorHandler.smoke
  );
  server.sendHeader("Access-Control-Allow-Origin", "*");
  sendHeaderNoCache();
  server.send(200, "text/plain", buffer );
}

void handlePlainTempC() { handlePlainFloatValue(sensorHandler.tempC); }
void handlePlainTempF() { handlePlainFloatValue(sensorHandler.tempF); }
void handlePlainHumidity() { handlePlainFloatValue(sensorHandler.humidity); }
void handlePlainLPG() { handlePlainFloatValue(sensorHandler.lpg); }
void handlePlainCO() { handlePlainFloatValue(sensorHandler.co); }
void handlePlainSmoke() { handlePlainFloatValue(sensorHandler.smoke); }
