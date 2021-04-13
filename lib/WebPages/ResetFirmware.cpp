#include "WebPages.h"

void handleResetFirmware()
{
  sendAuthentication();
  sendHeader(APP_NAME " - Reset Firmware", true);
  sendPrint("<form class='pure-form'>");
  sendLegend("Reset Firmware.");

  if (strcmp("true", server.arg(0).c_str()) == 0)
  {
    sendPrint("<h4>Resetting firmware... restart takes about 30sec.</h4>");
    app.firmwareReset();
  }
  else
  {
    sendPrint("If you really want to reset to system defaults, you must select 'Yes' on the maintenance page.");
  }

  sendPrint("</form>");
  sendFooter();
}
