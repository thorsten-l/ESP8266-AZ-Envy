#include "App.hpp"
#include "DefaultAppConfig.h"
#include <FS.h>
#include <LittleFS.h>
#include <MicroJson.hpp>
#include <Util.hpp>
#include <Development.h>
#include <TelnetStream.h>

struct tm appTimeinfo;
char appUptimeBuffer[64];
char appLocalIpBuffer[32];
char appDateTimeBuffer[32];

App app;
AppConfig appcfg;
AppConfig appcfgWR;
AppConfig appcfgRD;

char *formatChipId( char *attribute )
{
  snprintf( buffer, 63, attribute, ESP.getChipId());
  buffer[63]= 0;
  strncpy(attribute,buffer,63);
  return attribute;
}

const char *appUptime()
{
  time_t uptime = millis() / 1000;
  int uptimeSeconds = uptime % 60;
  int uptimeMinutes = (uptime / 60) % 60;
  int uptimeHours = (uptime / 3600) % 24;
  time_t uptimeDays = (uptime / 86400);
  sprintf(appUptimeBuffer, "%ld days, %d hours, %d minutes, %d seconds", uptimeDays, uptimeHours, uptimeMinutes,
          uptimeSeconds);
  return appUptimeBuffer;
}

void appShowHeader(Stream &out)
{
  out.println("\n\n" APP_NAME " - " APP_VERSION " - " APP_AUTHOR);
  out.println("BUILD: " __DATE__ " " __TIME__);
  out.println("PIOENV: " PIOENV);
  out.println("PIOPLATFORM: " PIOPLATFORM);
  out.println("PIOFRAMEWORK: " PIOFRAMEWORK);
  out.printf("ESP SDK Version: %s\n", ESP.getSdkVersion());
  out.printf("ESP Core Version: %s\n\n", ESP.getCoreVersion().c_str());
}

App::App()
{
  sprintf(initFilename, "/%08X.ini", ESP.getChipId());
  defaultConfig();
  initSPIFFS = false;
  initialized = true;
  doSystemRestart = false;
  ledActiveMode = false;
  ledNightMode = false;
}

void App::defaultConfig()
{
  strncpy(appcfg.wifi_ssid, DEFAULT_WIFI_SSID, 63);
  strncpy(appcfg.wifi_password, DEFAULT_WIFI_PASSWORD, 63);
  appcfg.wifi_mode = DEFAULT_WIFI_MODE;

  appcfg.net_mode = DEFAULT_NET_MODE;
  strncpy(appcfg.net_host, DEFAULT_NET_HOST, 63);
  strncpy(appcfg.net_mask, DEFAULT_NET_MASK, 63);
  strncpy(appcfg.net_gateway, DEFAULT_NET_GATEWAY, 63);
  strncpy(appcfg.net_dns, DEFAULT_NET_DNS, 63);

  strncpy(appcfg.ota_hostname, DEFAULT_OTA_HOSTNAME, 63);
  strncpy(appcfg.ota_password, DEFAULT_OTA_PASSWORD, 63);

  strncpy(appcfg.admin_password, DEFAULT_ADMIN_PASSWORD, 63);
  appcfg.mqtt_enabled = DEFAULT_MQTT_ENABLED;
  strncpy(appcfg.mqtt_clientid, DEFAULT_MQTT_CLIENTID, 63);
  strncpy(appcfg.mqtt_host, DEFAULT_MQTT_HOST, 63);
  appcfg.mqtt_port = DEFAULT_MQTT_PORT;
  appcfg.mqtt_useauth = DEFAULT_MQTT_USEAUTH;
  strncpy(appcfg.mqtt_user, DEFAULT_MQTT_USER, 63);
  strncpy(appcfg.mqtt_password, DEFAULT_MQTT_PASSWORD, 63);

  appcfg.syslog_enabled = DEFAULT_SYSLOG_ENABLED;
  strncpy(appcfg.syslog_host, DEFAULT_SYSLOG_HOST, 63);
  appcfg.syslog_port = DEFAULT_SYSLOG_PORT;
  strncpy(appcfg.syslog_app_name, DEFAULT_SYSLOG_APP_NAME, 63);

  strncpy(appcfg.mqtt_topic_temp_c, DEFAULT_MQTT_TOPIC_TEMP_C, 63);
  strncpy(appcfg.mqtt_topic_temp_f, DEFAULT_MQTT_TOPIC_TEMP_F, 63);
  strncpy(appcfg.mqtt_topic_humidity, DEFAULT_MQTT_TOPIC_HUMIDITY, 63);
  strncpy(appcfg.mqtt_topic_lpg, DEFAULT_MQTT_TOPIC_LPG, 63);
  strncpy(appcfg.mqtt_topic_co, DEFAULT_MQTT_TOPIC_CO, 63);
  strncpy(appcfg.mqtt_topic_smoke, DEFAULT_MQTT_TOPIC_SMOKE, 63);
  strncpy(appcfg.mqtt_topic_json, DEFAULT_MQTT_TOPIC_JSON, 63);

  appcfg.mqtt_sending_interval = DEFAULT_MQTT_SENDING_INTERVAL;

  appcfg.offset_c = DEFAULT_OFFSET_C;
  appcfg.offset_f = DEFAULT_OFFSET_F;
  appcfg.factor_lpg = DEFAULT_FACTOR_LPG;
  appcfg.factor_co = DEFAULT_FACTOR_CO;
  appcfg.factor_smoke = DEFAULT_FACTOR_SMOKE;

#ifdef WIFI_LED
  appcfg.led_night_mode_enabled = DEFAULT_LED_NIGHT_MODE_ENABLED;
  appcfg.led_night_mode_timeout = DEFAULT_LED_NIGHT_MODE_TIMEOUT;
#endif

  appcfg.telnet_enabled = DEFAULT_TELNET_ENABLED;

  memcpy(&appcfgWR, &appcfg, sizeof(appcfg));
  memcpy(&appcfgRD, &appcfg, sizeof(appcfg));
}

void App::firmwareReset()
{
  if (LittleFS.begin())
  {
    LOG0("Removing init file\n");
    LittleFS.remove(initFilename);
    LittleFS.end();
  }
  delayedSystemRestart();
}

void App::formatSPIFFS()
{
  wifiLedOn();
  ESP.eraseConfig();

  if (LittleFS.begin())
  {
    LOG0("File system format started...\n");
    LittleFS.format();
    LOG0("File system format finished.\n");
    LittleFS.end();
  }
  else
  {
    LOG0("\nERROR: format filesystem.\n");
  }
  wifiLedOff();
}

void App::restartSystem()
{
  // watchdogTicker.detach();
  ESP.eraseConfig();
  LOG0("*** restarting system ***\n");
  delay(2000);
  ESP.restart();
  delay(2000);
  ESP.reset();
}

void App::setup()
{
  Serial.begin(115200);

  pinMode(POWER_BUTTON, INPUT_PULLUP);

#ifdef WIFI_LED
  pinMode(WIFI_LED, OUTPUT);
  wifiLedOff();
#endif

  for (int i = 0; i < 5; i++)
  {
    wifiLedOn();
    delay(500);
    wifiLedOff();
    delay(500);
    Serial.println();
  }

  Serial.println("\n\n");
  Serial.println("\n\n");
  Serial.println(F(APP_NAME ", Version " APP_VERSION ", by " APP_AUTHOR));
  Serial.println("Build date: " __DATE__ " " __TIME__);
  Serial.printf("appcfg file size: %d bytes\n\n", sizeof(appcfg));

  showChipInfo();

  if (LittleFS.begin())
  {
    if (LittleFS.exists(initFilename))
    {
      LOG1("Init file %s found.\n", initFilename);
      initSPIFFS = false;
    }
    else
    {
      LOG1("WARNING: Init file %s does not exist.\n", initFilename);
      initSPIFFS = true;
    }

    LittleFS.end();
  }
  else
  {
    initSPIFFS = true;
  }

#ifndef POWER_BUTTON_IS_MULTIMODE
  if (digitalRead(POWER_BUTTON) == false)
  {
    Serial.println();
    LOG0("*** Firmware RESET ***\n");
    Serial.println();

    for (int i = 0; i < 15; i++)
    {
      wifiLedOn();
      delay(100);
      wifiLedOff();
      delay(100);
    }

    initSPIFFS = true;
  }
#endif

  if (initSPIFFS == true)
  {
    formatSPIFFS();

    if (LittleFS.begin())
    {
      LOG0("writing init file\n");
      File initFile = LittleFS.open(initFilename, "w");
      initFile.write("true");
      initFile.close();
      LittleFS.end();
    }

    restartSystem();
  }

  ESP.eraseConfig();
  memcpy(&appcfgRD, &appcfg, sizeof(appcfg));
  loadConfig();

///////////////////////////////////////////////////////////////
#ifdef OVERRIDE_WIFI_SETTINGS
  appcfg.wifi_mode = OVERRIDE_WIFI_MODE;
  strcpy( appcfg.wifi_ssid, OVERRIDE_WIFI_SSID );
  strcpy( appcfg.wifi_password, OVERRIDE_WIFI_PASSWORD );
  appcfg.ota_enabled = OVERRIDE_OTA_ENABLED;
  strcpy( appcfg.ota_password, OVERRIDE_OTA_PASSWORD );
#endif
///////////////////////////////////////////////////////////////
  
  formatChipId( appcfg.ota_hostname );
  formatChipId( appcfg.mqtt_clientid );
  formatChipId( appcfg.mqtt_topic_temp_c );
  formatChipId( appcfg.mqtt_topic_temp_f );
  formatChipId( appcfg.mqtt_topic_humidity );
  formatChipId( appcfg.mqtt_topic_lpg );
  formatChipId( appcfg.mqtt_topic_co );
  formatChipId( appcfg.mqtt_topic_smoke );
  formatChipId( appcfg.mqtt_topic_json );

  memcpy(&appcfgWR, &appcfg, sizeof(appcfg));
}

void App::loadConfig()
{
  if (!LittleFS.begin())
  {
    LOG0("ERROR: Failed to mount file system");
  }
  else
  {
    if (LittleFS.exists(APP_CONFIG_FILE_JSON))
    {
      if (loadJsonConfig(APP_CONFIG_FILE_JSON) == false)
      {
        memcpy(&appcfg, &appcfgRD, sizeof(appcfg));
      }
      else
      {
        LOG0("ERROR: loading config.json file. Using default config.\n");
      }
    }
    else
    {
      LOG0("WARNING: appcfg file " APP_CONFIG_FILE_JSON
           " does not exist. Using default appcfg.\n");
    }
    LittleFS.end();
  }
}

void App::writeConfig()
{
  if (!LittleFS.begin())
  {
    LOG0("ERROR: Failed to mount file system");
  }
  else
  {
    LOG0("Writing " APP_CONFIG_FILE_JSON " file.\n");
    File configJson = LittleFS.open(APP_CONFIG_FILE_JSON, "w");
    uJson j = uJson(configJson);

    j.writeHeader();
    j.writeEntry(A_wifi_ssid, appcfgWR.wifi_ssid);
    j.writeEntry(A_wifi_password, appcfgWR.wifi_password);
    j.writeEntry(A_wifi_mode, appcfgWR.wifi_mode);

    j.writeEntry(A_net_mode, appcfgWR.net_mode);
    j.writeEntry(A_net_host, appcfgWR.net_host);
    j.writeEntry(A_net_mask, appcfgWR.net_mask);
    j.writeEntry(A_net_gateway, appcfgWR.net_gateway);
    j.writeEntry(A_net_dns, appcfgWR.net_dns);

    j.writeEntry(A_ota_enabled, appcfgWR.ota_enabled);
    j.writeEntry(A_ota_hostname, appcfgWR.ota_hostname);
    j.writeEntry(A_ota_password, appcfgWR.ota_password);

    j.writeEntry(A_admin_password, appcfgWR.admin_password);

    j.writeEntry(A_mqtt_enabled, appcfgWR.mqtt_enabled);
    j.writeEntry(A_mqtt_clientid, appcfgWR.mqtt_clientid);
    j.writeEntry(A_mqtt_host, appcfgWR.mqtt_host);
    j.writeEntry(A_mqtt_port, appcfgWR.mqtt_port);
    j.writeEntry(A_mqtt_useauth, appcfgWR.mqtt_useauth);
    j.writeEntry(A_mqtt_user, appcfgWR.mqtt_user);
    j.writeEntry(A_mqtt_password, appcfgWR.mqtt_password);

    j.writeEntry(A_mqtt_topic_temp_c, appcfgWR.mqtt_topic_temp_c);
    j.writeEntry(A_mqtt_topic_temp_f, appcfgWR.mqtt_topic_temp_f);
    j.writeEntry(A_mqtt_topic_humidity, appcfgWR.mqtt_topic_humidity);
    j.writeEntry(A_mqtt_topic_lpg, appcfgWR.mqtt_topic_lpg);
    j.writeEntry(A_mqtt_topic_co, appcfgWR.mqtt_topic_co);
    j.writeEntry(A_mqtt_topic_smoke, appcfgWR.mqtt_topic_smoke);
    j.writeEntry(A_mqtt_topic_json, appcfgWR.mqtt_topic_json);

    j.writeEntry(A_mqtt_sending_interval, appcfgWR.mqtt_sending_interval);

    j.writeEntry(A_syslog_enabled, appcfgWR.syslog_enabled);
    j.writeEntry(A_syslog_host, appcfgWR.syslog_host);
    j.writeEntry(A_syslog_port, appcfgWR.syslog_port);
    j.writeEntry(A_syslog_app_name, appcfgWR.syslog_app_name);

    j.writeEntry(A_offset_c, appcfgWR.offset_c);
    j.writeEntry(A_offset_f, appcfgWR.offset_f);
    j.writeEntry(A_factor_lpg, appcfgWR.factor_lpg);
    j.writeEntry(A_factor_co, appcfgWR.factor_co);
    j.writeEntry(A_factor_smoke, appcfgWR.factor_smoke);

#ifdef WIFI_LED
    j.writeEntry(A_led_night_mode_enabled, appcfgWR.led_night_mode_enabled);
    j.writeEntry(A_led_night_mode_timeout, appcfgWR.led_night_mode_timeout);
#endif

    j.writeEntry(A_telnet_enabled, appcfgWR.telnet_enabled);

    j.writeFooter();
    configJson.close();

    FSInfo fs_info;
    LittleFS.info(fs_info);

    fsTotalBytes = fs_info.totalBytes;
    fsUsedBytes = fs_info.usedBytes;

    Serial.printf("\n--- SPIFFS Info ---\ntotal bytes = %d\n",
                  fs_info.totalBytes);
    Serial.printf("used bytes = %d\n", fs_info.usedBytes);
    Serial.printf("block size = %d\n", fs_info.blockSize);
    Serial.printf("page size = %d\n", fs_info.pageSize);
    Serial.printf("max open files = %d\n", fs_info.maxOpenFiles);
    Serial.printf("max path length = %d\n", fs_info.maxPathLength);

    LittleFS.end();
  }
}

void App::printConfig(AppConfig ac)
{
  Serial.println();
  Serial.println("--- App appcfguration -----------------------------------");
  Serial.println("  Security:");
  Serial.printf("    Admin password: %s\n", ac.admin_password);
  Serial.println("\n  WiFi:");
  Serial.printf("    SSID: %s\n", ac.wifi_ssid);
  Serial.printf("    Password: %s\n", ac.wifi_password);
  Serial.printf("    Mode: %s\n",
                (ac.wifi_mode == 1) ? "Station" : "Access Point");
  Serial.println("\n  Network:");
  Serial.printf("    Mode: %s\n",
                (ac.net_mode == NET_MODE_DHCP) ? "DHCP" : "Static");
  Serial.printf("    host address: %s\n", ac.net_host);
  Serial.printf("    gateway: %s\n", ac.net_gateway);
  Serial.printf("    netmask: %s\n", ac.net_mask);
  Serial.printf("    dns server: %s\n", ac.net_dns);
  Serial.println("\n  OTA:");
  Serial.printf("    Enabled: %s\n", (ac.ota_enabled ? "true" : "false"));
  Serial.printf("    Hostname: %s\n", ac.ota_hostname);
  Serial.printf("    Password: %s\n", ac.ota_password);

  Serial.println("\n  MQTT:");
  Serial.printf("    Enabled: %s\n", (ac.mqtt_enabled ? "true" : "false"));
  Serial.printf("    Client ID: %s\n", ac.mqtt_clientid);
  Serial.printf("    Host: %s\n", ac.mqtt_host);
  Serial.printf("    Port: %d\n", ac.mqtt_port);
  Serial.printf("    Use Auth: %s\n", (ac.mqtt_useauth ? "true" : "false"));
  Serial.printf("    User: %s\n", ac.mqtt_user);
  Serial.printf("    Password: %s\n", ac.mqtt_password);

  Serial.printf("    Topic temp C: %s\n", ac.mqtt_topic_temp_c);
  Serial.printf("    Topic temp F: %s\n", ac.mqtt_topic_temp_f);
  Serial.printf("    Topic humidity: %s\n", ac.mqtt_topic_humidity);
  Serial.printf("    Topic LPG: %s\n", ac.mqtt_topic_lpg);
  Serial.printf("    Topic CO: %s\n", ac.mqtt_topic_co);
  Serial.printf("    Topic smoke: %s\n", ac.mqtt_topic_smoke);
  Serial.printf("    Topic JSON: %s\n", ac.mqtt_topic_json);

  Serial.printf("    Sending Interval: %ld\n", ac.mqtt_sending_interval);

  Serial.println("\n  Syslog:");
  Serial.printf("    Enabled: %s\n",
                (ac.syslog_enabled ? "true" : "false"));
  Serial.printf("    Host: %s\n", ac.syslog_host);
  Serial.printf("    Port: %d\n", ac.syslog_port);
  Serial.printf("    App Name: %s\n", ac.syslog_app_name);

#ifdef WIFI_LED
  Serial.println("\n  LED night mode:");
  Serial.printf("    Enabled: %s\n",
                (ac.led_night_mode_enabled ? "true" : "false"));
  Serial.printf("    led_night_mode_timeout: %ds\n", ac.led_night_mode_timeout);
#endif

  Serial.println("\n  Telnet:");
  Serial.printf("    Enabled: %s\n",
                (ac.telnet_enabled ? "true" : "false"));

  Serial.println("\n  SHT30:");
  Serial.printf("    Temp. C offset: %f\n", ac.offset_c );
  Serial.printf("    Temp. F offset: %f\n", ac.offset_f );

  Serial.println("\n  MQ2:");
  Serial.printf("    LPG factor: %f\n", ac.factor_lpg );
  Serial.printf("    CO factor: %f\n", ac.factor_co );
  Serial.printf("    Smoke factor: %f\n", ac.factor_smoke );

  Serial.println("---------------------------------------------------------");
  Serial.println();
}

void App::delayedSystemRestart()
{
  doSystemRestart = true;
  systemRestartTimestamp = millis();
  systemRestartCounter = 6;
  LOG0("*** delayedSystemRestart ***\n");
}

void App::handle(unsigned long timestamp)
{

  if (doSystemRestart && ((timestamp - systemRestartTimestamp) > 1000))
  {

    systemRestartCounter--;

    if (systemRestartCounter > 0)
    {
      LOG1("*** system restart in %us ***\n", systemRestartCounter);
    }
    else
    {
      LOG1("*** do system restart *** (%lu)\n", (timestamp - systemRestartTimestamp));
      writeConfig();
      restartSystem();
    }

    systemRestartTimestamp = millis();
  }

  updateLedStates(timestamp);
}

bool App::loadJsonConfig(const char *filename)
{
  bool readError = false;
  char attributeName[128];

  File tmpConfig = LittleFS.open(filename, "r");

  uJson j = uJson(tmpConfig);

  if (j.readHeader())
  {
    memcpy(&appcfgRD, &appcfg, sizeof(appcfg));

    while (readError == false && j.readAttributeName(attributeName) == true)
    {
      readError |= j.readEntryChars(attributeName, A_wifi_ssid, appcfgRD.wifi_ssid);
      readError |= j.readEntryChars(attributeName, A_wifi_password, appcfgRD.wifi_password);
      readError |= j.readEntryInteger(attributeName, A_wifi_mode, &appcfgRD.wifi_mode);

      readError |= j.readEntryInteger(attributeName, A_net_mode, &appcfgRD.net_mode);
      readError |= j.readEntryChars(attributeName, A_net_host, appcfgRD.net_host);
      readError |= j.readEntryChars(attributeName, A_net_mask, appcfgRD.net_mask);
      readError |= j.readEntryChars(attributeName, A_net_gateway, appcfgRD.net_gateway);
      readError |= j.readEntryChars(attributeName, A_net_dns, appcfgRD.net_dns);

      readError |= j.readEntryBoolean(attributeName, A_ota_enabled, &appcfgRD.ota_enabled);
      readError |= j.readEntryChars(attributeName, A_ota_hostname, appcfgRD.ota_hostname);
      readError |= j.readEntryChars(attributeName, A_ota_password, appcfgRD.ota_password);

      readError |= j.readEntryChars(attributeName, A_admin_password, appcfgRD.admin_password);

      readError |= j.readEntryBoolean(attributeName, A_mqtt_enabled, &appcfgRD.mqtt_enabled);
      readError |= j.readEntryChars(attributeName, A_mqtt_clientid, appcfgRD.mqtt_clientid);
      readError |= j.readEntryChars(attributeName, A_mqtt_host, appcfgRD.mqtt_host);
      readError |= j.readEntryInteger(attributeName, A_mqtt_port, &appcfgRD.mqtt_port);

      readError |= j.readEntryChars(attributeName, A_mqtt_topic_temp_c, appcfgRD.mqtt_topic_temp_c);
      readError |= j.readEntryChars(attributeName, A_mqtt_topic_temp_f, appcfgRD.mqtt_topic_temp_f);
      readError |= j.readEntryChars(attributeName, A_mqtt_topic_humidity, appcfgRD.mqtt_topic_humidity);
      readError |= j.readEntryChars(attributeName, A_mqtt_topic_lpg, appcfgRD.mqtt_topic_lpg);
      readError |= j.readEntryChars(attributeName, A_mqtt_topic_co, appcfgRD.mqtt_topic_co);
      readError |= j.readEntryChars(attributeName, A_mqtt_topic_smoke, appcfgRD.mqtt_topic_smoke);
      readError |= j.readEntryChars(attributeName, A_mqtt_topic_json, appcfgRD.mqtt_topic_json);

      readError |= j.readEntryBoolean(attributeName, A_mqtt_useauth, &appcfgRD.mqtt_useauth);
      readError |= j.readEntryChars(attributeName, A_mqtt_user, appcfgRD.mqtt_user);
      readError |= j.readEntryChars(attributeName, A_mqtt_password, appcfgRD.mqtt_password);

      readError |= j.readEntryULong(attributeName, A_mqtt_sending_interval, &appcfgRD.mqtt_sending_interval);

      readError |= j.readEntryBoolean(attributeName, A_syslog_enabled, &appcfgRD.syslog_enabled);
      readError |= j.readEntryChars(attributeName, A_syslog_host, appcfgRD.syslog_host);
      readError |= j.readEntryInteger(attributeName, A_syslog_port, &appcfgRD.syslog_port);
      readError |= j.readEntryChars(attributeName, A_syslog_app_name, appcfgRD.syslog_app_name);

#ifdef WIFI_LED
      readError |= j.readEntryBoolean(attributeName, A_led_night_mode_enabled, &appcfgRD.led_night_mode_enabled);
      readError |= j.readEntryInteger(attributeName, A_led_night_mode_timeout, &appcfgRD.led_night_mode_timeout);
#endif

      readError |= j.readEntryBoolean(attributeName, A_telnet_enabled, &appcfgRD.telnet_enabled);

      readError |= j.readEntryFloat(attributeName, A_offset_c, &appcfgRD.offset_c);
      readError |= j.readEntryFloat(attributeName, A_offset_f, &appcfgRD.offset_f);
      readError |= j.readEntryFloat(attributeName, A_factor_lpg, &appcfgRD.factor_lpg);
      readError |= j.readEntryFloat(attributeName, A_factor_co, &appcfgRD.factor_co);
      readError |= j.readEntryFloat(attributeName, A_factor_smoke, &appcfgRD.factor_smoke);
    }
  }

  tmpConfig.close();

  return readError;
}

void App::wifiLedOn()
{
#ifdef WIFI_LED
  wifiLedState = 1;
  digitalWrite(WIFI_LED, WIFI_LED_ON);
  ledStateTimestamp = millis();
#endif
}

void App::wifiLedOff()
{
#ifdef WIFI_LED
  wifiLedState = 0;
  digitalWrite(WIFI_LED, WIFI_LED_OFF);
  ledStateTimestamp = millis();
#endif
}

void App::wifiLedToggle()
{
#ifdef WIFI_LED
  if (wifiLedState == 1)
  {
    wifiLedOff();
  }
  else
  {
    wifiLedOn();
  }
#endif
}


void App::updateLedStates(unsigned long timestamp)
{
#ifdef WIFI_LED
  if (appcfg.led_night_mode_enabled == true)
  {
    if (timestamp < (ledStateTimestamp + (appcfg.led_night_mode_timeout * 1000)))
    {
      if (ledActiveMode == false)
      {
        ledNightMode = false;
        ledActiveMode = true;
        if (wifiLedState == 1)
        {
          digitalWrite(WIFI_LED, WIFI_LED_ON);
        }
      }
    }
    else
    {
      if (ledNightMode == false)
      {
        ledActiveMode = false;
        ledNightMode = true;
        digitalWrite(WIFI_LED, WIFI_LED_OFF);
      }
    }
  }
#endif
}

void App::showLeds()
{
  ledStateTimestamp = millis();
}
