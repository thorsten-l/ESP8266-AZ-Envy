#include "WebPages.h"

void handleRestoreConfiguration()
{
  sendAuthentication();
  sendHeader(APP_NAME " - Restore Config", true);
  sendPrint("<form class='pure-form'>");
  sendLegend("Configuration successfully restored.");
  sendPrint("<h4 style='color: red'>Restarting System ... takes about 30s</h4></form>");
  sendFooter();

  if (app.loadJsonConfig(APP_CONFIG_FILE_JSON) == false)
  {
    memcpy(&appcfgWR, &appcfgRD, sizeof(appcfg));
  }

  app.delayedSystemRestart();
}
