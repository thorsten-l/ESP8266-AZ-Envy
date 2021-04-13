#ifndef __APP_H__
#define __APP_H__

#include <Arduino.h>
#include <FS.h>
#include <DeviceConfig.hpp>
#include <ConfigAttributes.hpp>

#define LOG0( format ) Serial.printf( "(%lu) " format, millis())
#define LOG1( format, ... ) Serial.printf( "(%lu) " format, millis(), ##__VA_ARGS__ )
#define TLOG0( format ) TelnetStream.printf( "(%ld) " format, millis())
#define TLOG1( format, ... ) TelnetStream.printf( "(%ld) " format, millis(), ##__VA_ARGS__ )

#define APP_NAME "AZ-Envy"
#define APP_AUTHOR "Dr. Thorsten Ludewig <t.ludewig@gmail.com>"
#define APP_CONFIG_FILE_JSON "/config.json"

// Network mode
#define NET_MODE_STATIC 1
#define NET_MODE_DHCP 2

// Power-Button Mode
#define POWER_BUTTON_MODE_SWITCH          1
#define POWER_BUTTON_MODE_TOGGLE          2
#define POWER_BUTTON_MODE_TOGGLE_SWITCH   3

extern void appShowHeader(Stream& out);
extern const char *appUptime();
// extern const char *appDateTime();

typedef struct appconfig
{
  char wifi_ssid[64];
  char wifi_password[64];
  int  wifi_mode;

  int net_mode;
  char net_host[64];
  char net_mask[64];
  char net_gateway[64];
  char net_dns[64];

  bool ota_enabled;
  char ota_hostname[64];
  char ota_password[64];

  char admin_password[64];

  bool mqtt_enabled;
  char mqtt_clientid[64];
  char mqtt_host[64];
  int  mqtt_port;
  bool mqtt_useauth;
  char mqtt_user[64];
  char mqtt_password[64];
  char mqtt_outtopic[64];

  char mqtt_topic_temp_c[64];
  char mqtt_topic_temp_f[64];
  char mqtt_topic_humidity[64];
  char mqtt_topic_lpg[64];
  char mqtt_topic_co[64];
  char mqtt_topic_smoke[64];
  char mqtt_topic_json[64];

  unsigned long mqtt_sending_interval;

  bool syslog_enabled;
  char syslog_host[64];
  int syslog_port;
  char syslog_app_name[64];

  float offset_c;
  float offset_f;
  float factor_lpg;
  float factor_co;
  float factor_smoke;

#ifdef WIFI_LED
  bool led_night_mode_enabled;
  int led_night_mode_timeout;  
#endif

} AppConfig;

class App
{
private:
  char initFilename[32];
  bool initialized = false;
  bool doSystemRestart;
  unsigned long systemRestartTimestamp;
  unsigned int systemRestartCounter;
  bool initSPIFFS = false;
  unsigned long ledStateTimestamp;
  int wifiLedState;
  int powerLedState;
  bool ledNightMode;
  bool ledActiveMode;

  void formatSPIFFS();
  void loadConfig();
  void restartSystem();
  void updateLedStates( unsigned long timestamp );

public:
  size_t fsTotalBytes;
  size_t fsUsedBytes;

  App();

  void setup();
  void firmwareReset();
  void defaultConfig();
  void writeConfig();
  bool loadJsonConfig( const char *filename );
  void printConfig(AppConfig ac);
  void delayedSystemRestart();
  void handle( unsigned long timestamp );
  //
  void showLeds();
  void wifiLedOn();
  void wifiLedOff();
  void wifiLedToggle();
};

extern App app;
extern AppConfig appcfg;
extern AppConfig appcfgWR;
extern AppConfig appcfgRD;

#endif
