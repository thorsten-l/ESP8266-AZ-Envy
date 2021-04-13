#include "WebPages.h"

void handleSystemRestart()
{
  sendAuthentication();
  sendHeader(APP_NAME " - System restart", true);
  sendPrint("<form class='pure-form'>");
  sendLegend("System restart.");
  sendPrint("<h4>Restart takes about 30sec.</h4>");
  sendPrint("</form>");
  sendFooter();
  ESP.restart();
}
