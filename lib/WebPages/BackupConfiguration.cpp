#include "WebPages.h"
#include <FS.h>
#include <LittleFS.h>

void handleBackupConfiguration()
{
  sendAuthentication();
  server.sendHeader( "Content-Disposition", 
    "attachment; filename=\"" APP_CONFIG_FILE_JSON "\"");
  LittleFS.begin();
  File configFile = LittleFS.open( APP_CONFIG_FILE_JSON, "r");
  server.streamFile(configFile, "application/json");
  configFile.close();
  LittleFS.end();
}
