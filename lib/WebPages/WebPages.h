#ifndef __WEB_PAGES_H__
#define __WEB_PAGES_H__

#include <App.hpp>
#include <DefaultAppConfig.h>
#include <ESP8266WebServer.h>

#define JSON_RELAY_ON 1
#define JSON_RELAY_OFF 2
#define JSON_RELAY_STATE 3

extern ESP8266WebServer server;

extern void handleRootPage();
extern void handleInfoPage();
extern void handleSetupPage();
extern void handleMaintenancePage();
extern void handleSaveConfigPage();
extern void handleBackupConfiguration();
extern void handleRestoreConfiguration();
extern void handleConfigFileUpload();
extern void handleFirmwareUpload();
extern void handleFirmwareUploadSuccess();
extern void handleResetFirmware();
extern void handleSystemRestart();

extern void handleJsonStatus();
extern void handlePlainStatus();
extern void handlePlainTempC();
extern void handlePlainTempF();
extern void handlePlainHumidity();
extern void handlePlainLPG();
extern void handlePlainCO();
extern void handlePlainSmoke();

extern void sendAuthentication();
extern void sendHeaderNoCache();
extern void sendHeader(const char *title);
extern void sendHeader(const char *title, bool sendMetaRefresh);
extern void sendHeader(const char *title, bool sendMetaRefresh, const char *style);
extern void sendFooter();
extern void sendPrintf(const char *format, ...);
extern void sendPrint(const char *message);
extern void sendLegend(const char *name);
extern void sendGroupLabel(int id, const char *label);
extern void sendTextGroupReadOnly(int id, const char *label, const char *value);

#endif
