#include <App.hpp>
#include <WifiHandler.hpp>
#include <ESP8266WebServer.h>
#include <SensorHandler.hpp>
#include <Util.hpp>
#include "WebPages.h"

extern ESP8266WebServer server;

void handleJsonStatus()
{
  sprintf( buffer, 
    "{"
      "\"temp_c\":%.02f,"
      "\"temp_f\":%.02f,"
      "\"humidity\":%.01f,"
      "\"lpg\":%.08f,"
      "\"co\":%.08f,"
      "\"smoke\":%.08f"
    "}\r\n",
    sensorHandler.tempC,
    sensorHandler.tempF,
    sensorHandler.humidity,
    sensorHandler.lpg,
    sensorHandler.co,
    sensorHandler.smoke
  );

  server.sendHeader("Access-Control-Allow-Origin", "*");
  sendHeaderNoCache();
  server.send(200, "application/json", buffer );
}

