#include "WebPages.h"

void handleInfoPage()
{
  sendHeader(APP_NAME " - Info");
  sendPrint("<form class='pure-form'>");
  sendLegend("Application");
  sendPrint(
      "<p>Name: " APP_NAME "</p>"
      "<p>Version: " APP_VERSION "</p>"
      "<p>PlatformIO Environment: " PIOENV "</p>"
      "<p>Author: Dr. Thorsten Ludewig &lt;t.ludewig@gmail.com></p>");

  sendLegend("Build");
  sendPrint("<p>Date: " __DATE__ "</p>"
            "<p>Time: " __TIME__ "</p>");

  sendLegend("RESTful API");

  sendPrint("<p><b>JSON API</b> - Returns status in JSON format</p>");

  sendPrintf(
      "<p><a href='http://%s/api/info'>http://%s/api/info</a> - ESP8266 Info</p>",
      WiFi.localIP().toString().c_str(), WiFi.localIP().toString().c_str());

  sendPrintf(
      "<p><a href='http://%s/api/json'>http://%s/api/json</a> - HTTP_GET</p>",
      WiFi.localIP().toString().c_str(), WiFi.localIP().toString().c_str());

  sendPrint("<p>&nbsp;</p>");
  sendPrint("<p><b>Plain text API</b> - Returns status in plain text format</p>");

  sendPrintf(
      "<p><a href='http://%s/api/plain'>http://%s/api/plain</a> - HTTP_GET</p>",
      WiFi.localIP().toString().c_str(), WiFi.localIP().toString().c_str());

  sendPrintf(
      "<p><a href='http://%s/api/plain/temp_c'>http://%s/api/plain/temp_c</a> - Temperature Celsius</p>",
      WiFi.localIP().toString().c_str(), WiFi.localIP().toString().c_str());

  sendPrintf(
      "<p><a href='http://%s/api/plain/temp_f'>http://%s/api/plain/temp_f</a> - Temperature Fahrenheit</p>",
      WiFi.localIP().toString().c_str(), WiFi.localIP().toString().c_str());

  sendPrintf(
      "<p><a href='http://%s/api/plain/humidity'>http://%s/api/plain/humidity</a> - Humidity</p>",
      WiFi.localIP().toString().c_str(), WiFi.localIP().toString().c_str());

  sendPrintf(
      "<p><a href='http://%s/api/plain/lpg'>http://%s/api/plain/lpg</a> - LPG</p>",
      WiFi.localIP().toString().c_str(), WiFi.localIP().toString().c_str());

  sendPrintf(
      "<p><a href='http://%s/api/plain/co'>http://%s/api/plain/co</a> - CO</p>",
      WiFi.localIP().toString().c_str(), WiFi.localIP().toString().c_str());

  sendPrintf(
      "<p><a href='http://%s/api/plain/smoke'>http://%s/api/plain/smoke</a> - Smoke</p>",
      WiFi.localIP().toString().c_str(), WiFi.localIP().toString().c_str());

  sendLegend("Services");

  sendPrintf("<p>OTA Enabled: %s</p>",
             (appcfg.ota_enabled) ? "true" : "false");
  sendPrintf("<p>MQTT Enabled: %s</p>",
             (appcfg.mqtt_enabled) ? "true" : "false");
//  sendPrintf("<p>Syslog Enabled: %s</p>",
//             (appcfg.syslog_enabled) ? "true" : "false");

  sendPrint("</form>");
  sendFooter();
}
