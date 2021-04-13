#ifndef __WIFI_HANDLER_H__
#define __WIFI_HANDLER_H__

#include "LinkedList.hpp"

class WifiHandler
{
private:
  bool connected;
  int connectCounter = 0;
  char networkBuffer[1024];
  const char* scanNetworks();
  byte mac[6];
  char macAddress[20];

public:
  void setup();
  const bool isReady();
  const bool isConnected();
  const bool isInStationMode();
  ListNode* getScannedNetworks();
  const bool handle( time_t timestamp );
  const char *getLocalIP();
  int getConnectCounter();
  const char* getMacAddress();
  const char* getPhyMode();
  const char *getHostname();
};

extern WifiHandler wifiHandler;

#endif
