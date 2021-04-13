#include "WebPages.h"

extern bool firmwareUploadFailed;
extern char *firmwareUploadErrorMessage;

void handleFirmwareUploadSuccess()
{
  sendAuthentication();
  sendHeader(APP_NAME " - Firmware upload", true);
  sendPrint("<form class='pure-form'>");

  if (firmwareUploadFailed)
  {
    sendLegend("Firmware upload FAILED!");
    sendPrint("<h4>ERROR: ");
    sendPrint(firmwareUploadErrorMessage);
    sendPrint(".</h4>");
    sendFooter();
  }
  else
  {
    sendLegend("Firmware successfully uploaded.");
    sendPrint("<h4 style='color: red'>Restarting System ... takes about 30s</h4></form>");
    sendFooter();
    delay(1000);
    ESP.restart();
  }
}
