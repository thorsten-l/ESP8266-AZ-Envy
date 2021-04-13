#include "WebPages.h"

#include <ConfigAttributes.hpp>
#include <LinkedList.hpp>
#include <WifiHandler.hpp>
#include <Util.hpp>
#include <html/setup.h>

const char *selected = "selected";
const char *checked = "checked";
const char *blank = "";

static const char *setupProcessor(const char *var)
{
  if (strcmp(var, A_admin_password) == 0)
    return appcfg.admin_password;

  // Wifi

  if (strcmp(var, "wifi_mode_ap") == 0 && appcfg.wifi_mode == WIFI_AP)
    return selected;
  if (strcmp(var, "wifi_mode_station") == 0 && appcfg.wifi_mode == WIFI_STA)
    return selected;

  if (strcmp(var, "scanned_network_options") == 0)
  {
    ListNode *node = wifiHandler.getScannedNetworks();
    String options = "";

    if (node == NULL)
    {
      LOG0("node==NULL\n");
    }

    while (node != NULL)
    {
      options += F("<option>");
      options += String(*node->value);
      options += F("</option>");
      node = node->next;
    }

    fillBuffer(options.c_str());
    return buffer;
  }

  if (strcmp(var, A_wifi_ssid) == 0)
    return appcfg.wifi_ssid;
  if (strcmp(var, A_wifi_password) == 0)
    return appcfg.wifi_password;

  // Network
  if (strcmp(var, "net_mode_dhcp") == 0 && appcfg.net_mode == NET_MODE_DHCP)
    return selected;
  if (strcmp(var, "net_mode_static") == 0 && appcfg.net_mode == NET_MODE_STATIC)
    return selected;
  if (strcmp(var, A_net_host) == 0)
    return appcfg.net_host;
  if (strcmp(var, A_net_gateway) == 0)
    return appcfg.net_gateway;
  if (strcmp(var, A_net_mask) == 0)
    return appcfg.net_mask;
  if (strcmp(var, A_net_dns) == 0)
    return appcfg.net_dns;

  // OTA
  if (strcmp(var, A_ota_hostname) == 0)
    return appcfg.ota_hostname;
  if (strcmp(var, A_ota_password) == 0)
    return appcfg.ota_password;
  if (strcmp(var, A_ota_enabled) == 0 && appcfg.ota_enabled == true)
    return checked;

  // MQTT
  if (strcmp(var, A_mqtt_enabled) == 0 && appcfg.mqtt_enabled == true)
    return checked;
  if (strcmp(var, A_mqtt_clientid) == 0)
    return appcfg.mqtt_clientid;
  if (strcmp(var, A_mqtt_host) == 0)
    return appcfg.mqtt_host;
  if (strcmp(var, A_mqtt_port) == 0)
  {
    sprintf(buffer, "%d", appcfg.mqtt_port);
    return buffer;
  }

  if (strcmp(var, A_mqtt_useauth) == 0 && appcfg.mqtt_useauth == true)
    return checked;
  if (strcmp(var, A_mqtt_user) == 0)
    return appcfg.mqtt_user;
  if (strcmp(var, A_mqtt_password) == 0)
    return appcfg.mqtt_password;

  if (strcmp(var, A_mqtt_topic_temp_c) == 0)
    return appcfg.mqtt_topic_temp_c;
  if (strcmp(var, A_mqtt_topic_temp_f) == 0)
    return appcfg.mqtt_topic_temp_f;
  if (strcmp(var, A_mqtt_topic_humidity) == 0)
    return appcfg.mqtt_topic_humidity;
  if (strcmp(var, A_mqtt_topic_lpg) == 0)
    return appcfg.mqtt_topic_lpg;
  if (strcmp(var, A_mqtt_topic_co) == 0)
    return appcfg.mqtt_topic_co;
  if (strcmp(var, A_mqtt_topic_smoke) == 0)
    return appcfg.mqtt_topic_smoke;
  if (strcmp(var, A_mqtt_topic_json) == 0)
    return appcfg.mqtt_topic_json;

  if (strcmp(var, A_mqtt_sending_interval) == 0)
  {
    sprintf(buffer, "%lu", appcfg.mqtt_sending_interval);
    return buffer;
  }

  if (strcmp(var, A_offset_c) == 0) { sprintf(buffer, "%0.1f", appcfg.offset_c); return buffer; }
  if (strcmp(var, A_offset_f) == 0) { sprintf(buffer, "%0.1f", appcfg.offset_f); return buffer; }
  if (strcmp(var, A_factor_lpg) == 0) { sprintf(buffer, "%0.1f", appcfg.factor_lpg); return buffer; }
  if (strcmp(var, A_factor_co) == 0) { sprintf(buffer, "%0.1f", appcfg.factor_co); return buffer; }
  if (strcmp(var, A_factor_smoke) == 0) { sprintf(buffer, "%0.1f", appcfg.factor_smoke); return buffer; }

  // Syslog
  if (strcmp(var, A_syslog_enabled) == 0 && appcfg.syslog_enabled == true)
    return checked;
  if (strcmp(var, A_syslog_host) == 0)
    return appcfg.syslog_host;
  if (strcmp(var, A_syslog_port) == 0)
  {
    sprintf(buffer, "%d", appcfg.syslog_port);
    return buffer;
  }
  if (strcmp(var, A_syslog_app_name) == 0)
    return appcfg.syslog_app_name;
  if (strcmp(var, "millis") == 0)
  {
    sprintf(buffer, "%lu", millis());
    return buffer;
  }



#ifdef WIFI_LED
  if (strcmp(var, A_led_night_mode_enabled) == 0 && appcfg.led_night_mode_enabled == true)
    return checked;
  if (strcmp(var,A_led_night_mode_timeout) == 0)
  {
    sprintf(buffer, "%d", appcfg.led_night_mode_timeout);
    return buffer;
  }
#endif

  return nullptr;
}

void handleSetupPage()
{
  sendAuthentication();
  sendHeader(APP_NAME " - Setup");
  sendHtmlTemplate(SETUP_HTML_TEMPLATE, setupProcessor);
  sendFooter();
}
