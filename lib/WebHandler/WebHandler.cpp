#include "WebHandler.hpp"
#include <App.hpp>
#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <Esp.h>
#include <FS.h>
#include <LittleFS.h>
#include <OtaHandler.hpp>
#include <Util.hpp>
#include <WifiHandler.hpp>
#include <WebPages.h>
#include <SensorHandler.hpp>

#include <uzlib.h>

#include <html/header_gz.h>
#include <html/footer_gz.h>
#include <html/header2_gz.h>
#include <html/header3_gz.h>
#include <html/setup.h>
#include <html/favicon_ico.h>

const char META_REFRESH30[] PROGMEM =
  "<meta http-equiv=\"refresh\" content=\"35; URL=/\">";

static struct uzlib_uncomp uzLibDecompressor;
static uint32_t uzlib_bytesleft;

size_t fsTotalBytes;
size_t fsUsedBytes;

WebHandler webHandler;
ESP8266WebServer server(80);

const char *getJsonStatus(WiFiClient *client)
{
  int remotePort = 0;
  char remoteAddress[32] = {0};

  if (client != NULL)
  {
    remotePort = client->remotePort();
    strncpy(remoteAddress, client->remoteIP().toString().c_str(), 31);
  }
  else
  {
    remotePort = server.client().remotePort();
    strncpy(remoteAddress, server.client().remoteIP().toString().c_str(), 31);
  }

  int wifi_rssi = WiFi.RSSI();
  int wifi_signal = (wifi_rssi < -100) ? -100 : wifi_rssi;
  wifi_signal = (wifi_signal > -50) ? 100 : 2 * (wifi_signal+100);

  sprintf(buffer,
          "{"
          "\"millis\":%lu,"
          "\"uptime\":\"%s\","
          "\"host_name\":\"%s.local\","
          "\"esp_full_version\":\"%s\","
          "\"esp_core_version\":\"%s\","
          "\"esp_sdk_version\":\"%s\","
          "\"platformio_env\":\"%s\","
          "\"platformio_platform\":\"%s\","
          "\"platformio_framework\":\"%s\","
          "\"arduino_board\":\"%s\","
          "\"chip_id\":\"%08X\","
          "\"cpu_freq\":\"%dMhz\","
          "\"flash_size\":%u,"
          "\"flash_speed\":%u,"
          "\"ide_size\":%u,"
          "\"fw_name\":\"%s\","
          "\"fw_version\":\"%s\","
          "\"build_date\":\"%s\","
          "\"build_time\":\"%s\","
          "\"wifi_ssid\":\"%s\","
          "\"wifi_reconnect_counter\":%d,"
          "\"wifi_channel\":%d,"
          "\"wifi_rssi\":%d,"
          "\"wifi_signal\":%d,"
          "\"wifi_phy_mode\":\"%s\","
          "\"wifi_mac_address\":\"%s\","
          "\"wifi_hostname\":\"%s\","
          "\"wifi_ip_address\":\"%s\","
          "\"wifi_gateway_ip\":\"%s\","
          "\"wifi_subnet_mask\":\"%s\","
          "\"wifi_dns_ip\":\"%s\","
          "\"spiffs_total\":%u,"
          "\"spiffs_used\":%u,"
          "\"free_heap\":%u,"
          "\"sketch_size\":%u,"
          "\"free_sketch_space\":%u,"
          "\"remote_client_ip\":\"%s\","
          "\"remote_client_port\":%u"
          "}",
          millis(), appUptime(), wifiHandler.getHostname(),
          ESP.getFullVersion().c_str(), ESP.getCoreVersion().c_str(), 
          ESP.getSdkVersion(), PIOENV, PIOPLATFORM, PIOFRAMEWORK, 
          ARDUINO_BOARD, ESP.getChipId(), ESP.getCpuFreqMHz(), 
          ESP.getFlashChipRealSize(), ESP.getFlashChipSpeed(), 
          ESP.getFlashChipSize(), APP_NAME, APP_VERSION, __DATE__, __TIME__, 
          appcfg.wifi_ssid, wifiHandler.getConnectCounter(), WiFi.channel(),
          wifi_rssi, wifi_signal,
          wifiHandler.getPhyMode(), wifiHandler.getMacAddress(),
          WiFi.hostname().c_str(), WiFi.localIP().toString().c_str(), 
          WiFi.gatewayIP().toString().c_str(), 
          WiFi.subnetMask().toString().c_str(), 
          WiFi.dnsIP().toString().c_str(), fsTotalBytes, fsUsedBytes,
          ESP.getFreeHeap(), ESP.getSketchSize(), ESP.getFreeSketchSpace(), 
          remoteAddress, remotePort );

  return buffer;
}

int sendUncompressed( const uint8_t *compressedData, const uint32_t compressedDataLength)
{
  uzlib_init();
  uzLibDecompressor.source = compressedData;
  uzLibDecompressor.source_limit = compressedData + compressedDataLength;
  
  uzlib_bytesleft = 0;
  for( int i=1; i<5; i++ )
  {
    uzlib_bytesleft <<= 8;
    uzlib_bytesleft |= pgm_read_byte( compressedData + compressedDataLength - i );
  }
  // Serial.printf("decompressed file length = %u\n", uzlib_bytesleft );
  
  uzlib_uncompress_init(&uzLibDecompressor, buffer, BUFFER_LENGTH );
  
  int res = uzlib_gzip_parse_header(&uzLibDecompressor);
  
  if (res != 0) {
    Serial.println("[ERROR] in gzUncompress: uzlib_gzip_parse_header failed!");
    return res;
  }

  while( uzlib_bytesleft > 0 ) {
    uzLibDecompressor.dest_start = (unsigned char *)buffer2;
    uzLibDecompressor.dest = (unsigned char *)buffer2;
    int to_read = ( uzlib_bytesleft > BUFFER2_LENGTH) ? BUFFER2_LENGTH : uzlib_bytesleft;
    uzLibDecompressor.dest_limit = (unsigned char *)buffer2 + to_read;
    uzlib_uncompress(&uzLibDecompressor);
    buffer2[to_read] = 0;
    server.sendContent(buffer2);
    uzlib_bytesleft -= to_read;
  }
  return 0;
}

void sendHeader(const char *title, bool sendMetaRefresh, const char *style)
{
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.chunkedResponseModeStart(200, "text/html");

  sendUncompressed(header_html_gz, header_html_gz_len);

  if (sendMetaRefresh)
  {
    server.sendContent_P(META_REFRESH30);
  }

  if (style != nullptr)
  {
    server.sendContent_P(style);
  }

  sprintf(buffer, "<title>%s</title>\n", title);
  server.sendContent(buffer);
  sendUncompressed(header2_html_gz, header2_html_gz_len);
  server.sendContent(title);
  sendUncompressed(header3_html_gz, header3_html_gz_len);
}

void sendHeader(const char *title, bool sendMetaRefresh)
{
  sendHeader(title, sendMetaRefresh, nullptr);
}

void sendHeader(const char *title)
{
  sendHeader(title, false);
}

void sendHeaderNoCache()
{
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "0");
}

void sendAuthentication()
{
  if (!server.authenticate("admin", appcfg.admin_password))
  {
    return server.requestAuthentication();
  }
  sendHeaderNoCache();
}

void sendFooter()
{
  sendUncompressed(footer_html_gz, footer_html_gz_len);
  server.chunkedResponseFinalize();
  server.client().stop();
}

void sendPrintf(const char *format, ...)
{
  va_list myargs;
  va_start(myargs, format);
  vsprintf(buffer, format, myargs);
  va_end(myargs);
  server.sendContent(buffer);
}

void sendPrint(const char *message)
{
  server.sendContent(message);
}

void sendLegend(const char *name)
{
  sendPrintf("<legend>%s</legend>", name);
}

void sendGroupLabel( int id, const char *label )
{
  sendPrintf(
    "<div class='pure-control-group'>"
      "<label for='pgid%d'>%s</label>", id, label );
}

void sendTextGroupReadOnly( int id, const char *label, const char *value )
{
  sendGroupLabel( id, label );
  sendPrintf(
      "<input id='pgid%d' type='text' maxlength='64' readonly value='%s'>"
    "</div>", id, value );
}

WebHandler::WebHandler() 
{ 
  initialized = false; 
}

void WebHandler::setup()
{
  LOG0("HTTP server setup...\n");

  uploadIsRunning = false;
  fsTotalBytes = 0;
  fsUsedBytes = 0;

  if (LittleFS.begin())
  {
    FSInfo fs_info;
    LittleFS.info(fs_info);
    fsTotalBytes = fs_info.totalBytes;
    fsUsedBytes = fs_info.usedBytes;
    LittleFS.end();
  }

  server.on("/api/info", []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    sendHeaderNoCache();
    String message(getJsonStatus(NULL));
    server.send(200, "application/json", message);
    server.client().stop();
  });

  server.on("/api/calibrate", []() {
    sensorHandler.calibrate();
    handleJsonStatus();
  });

  server.on("/favicon.ico", []() {
    server.sendHeader("Content-Encoding", "gzip");
    server.sendHeader("Cache-Control", "max-age=31536000");
    server.send( 200, "image/x-icon", FAVICON_ICO_GZ, FAVICON_ICO_GZ_LEN );
    server.client().stop();
  });

  server.on("/api/json", handleJsonStatus );
  server.on("/api/plain", handlePlainStatus );
  server.on("/api/plain/temp_c", handlePlainTempC);
  server.on("/api/plain/temp_f", handlePlainTempF);
  server.on("/api/plain/humidity", handlePlainHumidity);
  server.on("/api/plain/lpg", handlePlainLPG);
  server.on("/api/plain/co", handlePlainCO);
  server.on("/api/plain/smoke", handlePlainSmoke);

  server.on("/", handleRootPage);
  server.on("/info.html", handleInfoPage);
  server.on("/setup.html", handleSetupPage );
  server.on("/maintenance.html", handleMaintenancePage );
  server.on("/system-restart", handleSystemRestart );
  server.on("/config-backup", handleBackupConfiguration);
  server.on("/config-restore", HTTP_POST, handleRestoreConfiguration, handleConfigFileUpload);
  server.on("/update-firmware", HTTP_POST, handleFirmwareUploadSuccess, handleFirmwareUpload);
  server.on("/reset-firmware", handleResetFirmware);
  server.on("/savecfg", handleSaveConfigPage);

  if ( appcfg.wifi_mode == WIFI_AP )
  {
    server.on("/captive.html", HTTP_GET, []() {
      server.send(200, "text/html", 
        F("Please enter <b><a target='_blank' href='http://az-envy.local'>http://az-envy.local</a></b> or <b><a target='_blank' href='http://192.168.192.1'>http://192.168.192.1</a></b> into your browser."));
      server.client().stop();
    });
  }

  server.onNotFound([]{
    if ( appcfg.wifi_mode == WIFI_AP )
    {
      server.sendHeader( "Location", "http://az-envy.local/captive.html", true );
      server.send( 302, "text/plain", "");
      server.client().stop();
    }
    else
    {
      sprintf( buffer, "<h1>ERROR(404): Page not found.</h1>URI=%s", server.uri().c_str());
      server.send(404, "text/html", buffer);
      server.client().stop();
    }
  });

  if (appcfg.ota_enabled == false)
  {
    MDNS.begin(appcfg.ota_hostname);
  }

  MDNS.addService("http", "tcp", 80);
  MDNS.addServiceTxt("http", "tcp", "path", "/");
  MDNS.addServiceTxt("http", "tcp", "fw_name", APP_NAME);
  MDNS.addServiceTxt("http", "tcp", "fw_version", APP_VERSION);

  if (appcfg.ota_enabled == false)
  {
    MDNS.update();
  }

  server.begin();
  LOG0("HTTP server started\n");
  initialized = true;
}

void WebHandler::handle()
{
  if (!initialized)
  {
    setup();
  }
  else
  {
    server.handleClient();
    MDNS.update();
  }
}

