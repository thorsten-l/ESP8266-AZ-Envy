#include <App.hpp>
#include <DefaultAppConfig.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <LinkedList.hpp>
#include "WifiHandler.hpp"

WifiHandler wifiHandler;

SimpleLinkedList wifiNetworkLists;
const char *phyModes[] = {"11B", "11G", "11N"};
char ipBuffer[32];
DNSServer dnsServer;

static time_t lastTimestamp;

static void wifiOff()
{
  app.wifiLedOff();
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  WiFi.setOutputPower(0.0f);
  WiFi.hostname(appcfg.ota_hostname);
}

static void wifiInitStationMode()
{
  WiFi.persistent(false);
  WiFi.disconnect(true);
  delay(200);
  WiFi.begin();
  WiFi.mode(WIFI_STA);
  WiFi.hostname(appcfg.ota_hostname);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);

  LOG0("Starting Wifi in Station Mode\n");
  LOG1("  - Connecting to %s\n", appcfg.wifi_ssid);

  if (appcfg.net_mode == NET_MODE_STATIC)
  {
    LOG0("use static ip address");
    IPAddress host;
    host.fromString(appcfg.net_host);
    IPAddress gateway;
    gateway.fromString(appcfg.net_gateway);
    IPAddress mask;
    mask.fromString(appcfg.net_mask);
    IPAddress dns;
    dns.fromString(appcfg.net_dns);
    WiFi.config(host, gateway, mask, dns);
    // DNS WiFi.dnsIP(dns); // platform <= espressif8266@1.8.0
  }
  else
  {
    LOG0("use dhcp server");
  }

  WiFi.begin(appcfg.wifi_ssid, appcfg.wifi_password);
}

void WifiHandler::setup()
{
  LOG0("WiFi Setup started...\n");
  connected = false;
  lastTimestamp = 0;

  wifiOff();

  scanNetworks();

  wifiOff();

  if (isInStationMode())
  {
    wifiInitStationMode();
  }
  else
  {
    LOG0("Starting Wifi Access Point Mode\n");
    char buffer[64];
    sprintf(buffer, DEFAULT_WIFI_SSID, ESP.getChipId());
    strcpy(appcfg.wifi_ssid, buffer);

    WiFi.mode(WIFI_AP);

    IPAddress host;
    host.fromString(appcfg.net_host);
    IPAddress gateway;
    gateway.fromString(appcfg.net_gateway);
    IPAddress mask;
    mask.fromString(appcfg.net_mask);

    WiFi.softAPConfig(host, gateway, mask);
    WiFi.softAP(appcfg.wifi_ssid, appcfg.wifi_password);
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());

    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(53, "*", WiFi.softAPIP());
    LOG0( "DNS server started.\n" );

    app.wifiLedOn();
    connected = true;
  }

  WiFi.macAddress(mac);
  sprintf(macAddress, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0],
          mac[1], mac[2], mac[3], mac[4], mac[5]);
}

const bool WifiHandler::handle(time_t timestamp)
{
  if (isInStationMode())
  {
    if (timestamp - lastTimestamp >= 500)
    {
      lastTimestamp = timestamp;
      int status = WiFi.status();

      if (connected)
      {
        if (status == WL_CONNECTED)
        {
          return true;
        }
        else
        {
          LOG0("WARNING: WiFi connection lost!\n");

          wifiOff();
          wifiInitStationMode();

          connected = false;
        }
      }
      else
      {
        if (status == WL_CONNECTED)
        {
          Serial.println("\n");
          Serial.printf("WiFi connected to %s\n", appcfg.wifi_ssid);

          connectCounter++;

          if (appcfg.net_mode == NET_MODE_DHCP)
          {
            Serial.println("copy wifi config from dhcp response");
            strncpy(appcfg.net_host, WiFi.localIP().toString().c_str(), 63);
            strncpy(appcfg.net_gateway, WiFi.gatewayIP().toString().c_str(), 63);
            strncpy(appcfg.net_mask, WiFi.subnetMask().toString().c_str(), 63);
            strncpy(appcfg.net_dns, WiFi.dnsIP().toString().c_str(), 63);
          }

          Serial.printf(" - host ip address: %s\n", WiFi.localIP().toString().c_str());
          Serial.printf(" - gateway: %s\n", WiFi.gatewayIP().toString().c_str());
          Serial.printf(" - mask: %s\n", WiFi.subnetMask().toString().c_str());
          Serial.printf(" - dns server: %s\n", WiFi.dnsIP().toString().c_str());
          Serial.printf(" - WiFi Channel: %d\n", WiFi.channel());
          Serial.printf(" - WiFi phy mode: %s\n", getPhyMode());
          Serial.printf(" - WiFi MAC Address: %s\n", macAddress);
          Serial.printf(" - WiFi Hostname: %s\n", WiFi.hostname().c_str());

          Serial.println();
          app.wifiLedOn();
          connected = true;
          MDNS.update();
        }
        else
        {
          Serial.print(".");
          app.wifiLedToggle();
        }
      }
    }
  }
  else // AP-Mode
  {
    dnsServer.processNextRequest();
  }

  return connected;
}

const bool WifiHandler::isInStationMode()
{
  return (appcfg.wifi_mode == WIFI_STA);
}

const bool WifiHandler::isConnected()
{
  return connected;
}

const bool WifiHandler::isReady()
{
  return isConnected() && isInStationMode();
}

const char *WifiHandler::scanNetworks()
{
  networkBuffer[0] = 0;

  Serial.println("\nScanning WiFi networks...");
  int n = WiFi.scanNetworks(false, false);
  Serial.println("done.");

  if (n == 0)
  {
    Serial.println("no networks found");
    strcpy(networkBuffer, "no networks found");
  }
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    int l = 0;

    for (int i = 0; i < n; ++i)
    {
      l += sprintf(networkBuffer + l, "%2d: %s (%d)%s\n", i + 1,
                   WiFi.SSID(i).c_str(), WiFi.RSSI(i),
                   (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      wifiNetworkLists.put(WiFi.SSID(i).c_str());
      delay(5);
    }
  }

  Serial.println("----------------------------------------------");
  Serial.print(networkBuffer);
  Serial.println("----------------------------------------------");

  return networkBuffer;
}

ListNode *WifiHandler::getScannedNetworks()
{
  return wifiNetworkLists.getRootNode();
}

const char *WifiHandler::getLocalIP()
{
  strncpy(ipBuffer, WiFi.localIP().toString().c_str(), 31);
  ipBuffer[31] = 0;
  return ipBuffer;
}

int WifiHandler::getConnectCounter()
{
  return connectCounter;
}

const char *WifiHandler::getMacAddress()
{
  return macAddress;
}

const char *WifiHandler::getPhyMode()
{
  return phyModes[WiFi.getPhyMode() - 1];
}

const char *WifiHandler::getHostname()
{
  return appcfg.ota_hostname;
}
