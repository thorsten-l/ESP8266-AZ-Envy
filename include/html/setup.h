#include <App.hpp>

const char SETUP_HTML_TEMPLATE[] PROGMEM =
  "<!-- %millis% -->\n"
  "<form class=\"pure-form pure-form-aligned\" action=\"savecfg\" method=\"POST\">\n"

  "<div class='accordion'>Hardware</div>\n"
  "<div class='panel'>\n"
  "<fieldset>\n"

  "<div class='accordion'>LED Night Mode</div>\n"
  "<div class='panel sub-panel'>\n"
    "<div class=\"pure-control-group\"><label for=\"wlid1\">Enabled</label><input id=\"wlid1\" type=\"checkbox\" name=\"led_night_mode_enabled\" value=\"true\" %led_night_mode_enabled%></div>\n"
    "<div class=\"pure-control-group\"><label for=\"wlid2\">Timeout</label><input id=\"wlid2\" type=\"text\" name=\"led_night_mode_timeout\" maxlength=\"64\" value=\"%led_night_mode_timeout%\"></div>\n"
  "</div>\n"

  "<div class='accordion'>SHT30</div>\n"
  "<div class='panel sub-panel'>\n"
    "<div class=\"pure-control-group\"><label for=\"shid1\">Temp. C offset</label><input id=\"shid1\" type=\"text\" name=\"offset_c\" maxlength=\"64\" value=\"%offset_c%\"></div>\n"
    "<div class=\"pure-control-group\"><label for=\"shid2\">Temp. F offset</label><input id=\"shid2\" type=\"text\" name=\"offset_f\" maxlength=\"64\" value=\"%offset_f%\"></div>\n"
  "</div>\n"

  "<div class='accordion'>MQ2</div>\n"
  "<div class='panel sub-panel'>\n"
    "<div class=\"pure-control-group\"><label for=\"mqid1\">LPG factor</label><input id=\"mqid1\" type=\"text\" name=\"factor_lpg\" maxlength=\"64\" value=\"%factor_lpg%\"></div>\n"
    "<div class=\"pure-control-group\"><label for=\"mqid2\">CO factor</label><input id=\"mqid2\" type=\"text\" name=\"factor_co\" maxlength=\"64\" value=\"%factor_co%\"></div>\n"
    "<div class=\"pure-control-group\"><label for=\"mqid3\">Smoke factor</label><input id=\"mqid3\" type=\"text\" name=\"factor_smoke\" maxlength=\"64\" value=\"%factor_smoke%\"></div>\n"
  "</div>\n"

  "</fieldset></div>\n"

  "<div class='accordion'>Security</div>\n"
  "<div class='panel'>\n"
  "<fieldset>\n"
  
  "<div class='accordion'>Setup 'admin' user</div>\n"
  "<div class='panel sub-panel'>\n"
    "<div class=\"pure-control-group\"><label for=\"pgid0\">Password</label><input id=\"pgid0\" type=\"text\" name=\"admin_password\" maxlength=\"64\" value=\"%admin_password%\"></div>\n"
  "</div>\n"
  "</fieldset></div>\n"

  "<div class='accordion'>Network</div>\n"
  "<div class='panel'>\n"
  "<fieldset>\n"
  "<div class='accordion'>General</div>\n"
  "<div class='panel sub-panel'>\n"
    "<div class=\"pure-control-group\"><label for=\"pgid9\">Hostname</label><input id=\"pgid9\" type=\"text\" name=\"ota_hostname\" maxlength=\"64\" value=\"%ota_hostname%\"></div>\n"
  "</div>\n"

  "<div class='accordion'>WiFi</div>\n"
  "<div class='panel sub-panel'>\n"
    "<div class=\"pure-control-group\"><label for=\"pgid1\">Mode</label><select id=\"pgid1\" name=\"wifi_mode\"><option %wifi_mode_ap% value=\"2\">Access Point</option><option %wifi_mode_station% value=\"1\">Station</option></select></div>\n"
    "<div class=\"pure-control-group\"><label for=\"scannedNetwork\">Scanned Networks</label><select id=\"scannedNetwork\" onchange=\"document.getElementById('pgid2').value = this.value\"><option> </option>%scanned_network_options%</select></div>\n"
    "<div class=\"pure-control-group\"><label for=\"pgid2\">SSID</label><input id=\"pgid2\" type=\"text\" name=\"wifi_ssid\" maxlength=\"64\" value=\"%wifi_ssid%\"></div>\n"
    "<div class=\"pure-control-group\"><label for=\"pgid3\">Password</label><input id=\"pgid3\" type=\"text\" name=\"wifi_password\" maxlength=\"64\" value=\"%wifi_password%\"></div>\n"
  "</div>\n"

  "<div class='accordion'>Interface</div>\n"
  "<div class='panel sub-panel'>\n"
    "<div class=\"pure-control-group\"><label for=\"pgid4\">Mode</label><select id=\"pgid4\" name=\"net_mode\"><option %net_mode_dhcp% value=\"2\">DHCP</option><option %net_mode_static% value=\"1\">Static</option></select></div>\n"
    "<div class=\"pure-control-group\"><label for=\"pgid5\">Host Address</label><input id=\"pgid5\" type=\"text\" name=\"net_host\" maxlength=\"64\" value=\"%net_host%\"></div>\n"
    "<div class=\"pure-control-group\"><label for=\"pgid6\">Gateway</label><input id=\"pgid6\" type=\"text\" name=\"net_gateway\" maxlength=\"64\" value=\"%net_gateway%\"></div>\n"
    "<div class=\"pure-control-group\"><label for=\"pgid7\">Netmask</label><input id=\"pgid7\" type=\"text\" name=\"net_mask\" maxlength=\"64\" value=\"%net_mask%\"></div>\n"
    "<div class=\"pure-control-group\"><label for=\"pgid8\">DNS Server</label><input id=\"pgid8\" type=\"text\" name=\"net_dns\" maxlength=\"64\" value=\"%net_dns%\"></div>\n"
  "</div>\n"
  "</fieldset></div>\n"

  "<div class='accordion'>Services</div>\n"
  "<div class='panel'>\n"
  "<fieldset>\n"

  "<div class='accordion'>Over The Air - firmware update (OTA)</div>\n"
  "<div class='panel sub-panel'>\n"
    "<div class=\"pure-control-group\"><label for=\"ogid01\">Enabled</label><input id=\"ogid01\" type=\"checkbox\" name=\"ota_enabled\" value=\"true\" %ota_enabled%></div>\n"
    "<div class=\"pure-control-group\"><label for=\"ogid02\">Password</label><input id=\"ogid02\" type=\"text\" name=\"ota_password\" maxlength=\"64\" value=\"%ota_password%\"></div>\n"
  "</div>\n"

  "<div class='accordion'>MQTT</div>\n"
  "<div class='panel sub-panel'>\n"
    "<div class=\"pure-control-group\"><label for=\"pgid21\">Enabled</label><input id=\"pgid21\" type=\"checkbox\" name=\"mqtt_enabled\" value=\"true\" %mqtt_enabled%></div>\n"
    "<div class=\"pure-control-group\"><label for=\"pgid22\">Client ID</label><input id=\"pgid22\" type=\"text\" name=\"mqtt_clientid\" maxlength=\"64\" value=\"%mqtt_clientid%\"></div>\n"
    "<div class=\"pure-control-group\"><label for=\"pgid23\">Host</label><input id=\"pgid23\" type=\"text\" name=\"mqtt_host\" maxlength=\"64\" value=\"%mqtt_host%\"></div>\n"
    "<div class=\"pure-control-group\"><label for=\"pgid24\">Port</label><input id=\"pgid24\" type=\"text\" name=\"mqtt_port\" maxlength=\"64\" value=\"%mqtt_port%\"></div>\n"
    "<div class=\"pure-control-group\"><label for=\"pgid25\">Use Authentication</label><input id=\"pgid25\" type=\"checkbox\" name=\"mqtt_useauth\" value=\"true\" %mqtt_useauth%></div>\n"
    "<div class=\"pure-control-group\"><label for=\"pgid26\">User</label><input id=\"pgid26\" type=\"text\" name=\"mqtt_user\" maxlength=\"64\" value=\"%mqtt_user%\"></div>\n"
    "<div class=\"pure-control-group\"><label for=\"pgid27\">Password</label><input id=\"pgid27\" type=\"text\" name=\"mqtt_password\" maxlength=\"64\" value=\"%mqtt_password%\"></div>\n"

    "<div class=\"pure-control-group\"><label for=\"mtid01\">Topic temp C</label><input id=\"mtid01\" type=\"text\" name=\"mqtt_topic_temp_c\" maxlength=\"64\" value=\"%mqtt_topic_temp_c%\"></div>\n"
    "<div class=\"pure-control-group\"><label for=\"mtid02\">Topic temp F</label><input id=\"mtid02\" type=\"text\" name=\"mqtt_topic_temp_f\" maxlength=\"64\" value=\"%mqtt_topic_temp_f%\"></div>\n"
    "<div class=\"pure-control-group\"><label for=\"mtid03\">Topic humidity</label><input id=\"mtid03\" type=\"text\" name=\"mqtt_topic_humidity\" maxlength=\"64\" value=\"%mqtt_topic_humidity%\"></div>\n"
    "<div class=\"pure-control-group\"><label for=\"mtid04\">Topic LPG</label><input id=\"mtid04\" type=\"text\" name=\"mqtt_topic_lpg\" maxlength=\"64\" value=\"%mqtt_topic_lpg%\"></div>\n"
    "<div class=\"pure-control-group\"><label for=\"mtid05\">Topic CO</label><input id=\"mtid05\" type=\"text\" name=\"mqtt_topic_co\" maxlength=\"64\" value=\"%mqtt_topic_co%\"></div>\n"
    "<div class=\"pure-control-group\"><label for=\"mtid06\">Topic Smoke</label><input id=\"mtid06\" type=\"text\" name=\"mqtt_topic_smoke\" maxlength=\"64\" value=\"%mqtt_topic_smoke%\"></div>\n"
    "<div class=\"pure-control-group\"><label for=\"mtid07\">Topic JSON</label><input id=\"mtid07\" type=\"text\" name=\"mqtt_topic_json\" maxlength=\"64\" value=\"%mqtt_topic_json%\"></div>\n"

    "<div class=\"pure-control-group\"><label for=\"emid09\">Sending Interval</label><input id=\"emid09\" type=\"text\" name=\"mqtt_sending_interval\" maxlength=\"64\" value=\"%mqtt_sending_interval%\"></div>\n"
    "</div>\n"

/*
  "<div class='accordion'>Syslog</div>\n"
  "<div class='panel sub-panel'>\n"
    "<div class=\"pure-control-group\"><label for=\"pgid30\">Enabled</label><input id=\"pgid30\" type=\"checkbox\" name=\"syslog_enabled\" value=\"true\" %syslog_enabled%></div>\n"
    "<div class=\"pure-control-group\"><label for=\"pgid31\">Host</label><input id=\"pgid31\" type=\"text\" name=\"syslog_host\" maxlength=\"64\" value=\"%syslog_host%\"></div>\n"
    "<div class=\"pure-control-group\"><label for=\"pgid32\">Port (TCP)</label><input id=\"pgid32\" type=\"text\" name=\"syslog_port\" maxlength=\"64\" value=\"%syslog_port%\"></div>\n"
    "<div class=\"pure-control-group\"><label for=\"pgid33\">App Name</label><input id=\"pgid33\" type=\"text\" name=\"syslog_app_name\" maxlength=\"64\" value=\"%syslog_app_name%\"></div>\n"
  "</div>\n"
*/
  "<div class='accordion'>Telnet</div>\n"
  "<div class='panel sub-panel'>\n"
    "<div class=\"pure-control-group\"><label for=\"teid01\">Enabled</label><input id=\"teid01\" type=\"checkbox\" name=\"telnet_enabled\" value=\"true\" %telnet_enabled%></div>\n"
  "</div>\n"

  "</fieldset></div>\n"
  "\n<p><input class=\"pure-button pure-button-primary\" type=\"submit\" value=\"Save Configuration\"></p>\n"
  "</form>\n"

  "<p>&nbsp;</p>\n"
;
