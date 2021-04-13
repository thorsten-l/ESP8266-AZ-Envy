#include <Arduino.h>
#include <App.hpp>
#include <Util.hpp>
#include <html/maintenance.h>
#include "WebPages.h"

static const char *setupProcessor(const char *var)
{
  if (strcmp(var, "pioenv_name") == 0)
    return PIOENV_NAME;
  return nullptr;
}

void handleMaintenancePage()
{
  sendAuthentication();
  sendHeader(APP_NAME " - Maintenance", false, MAINTENANCE_STYLE);
  sendHtmlTemplate(MAINTENANCE_HTML_TEMPLATE, setupProcessor);
  sendFooter();
}
