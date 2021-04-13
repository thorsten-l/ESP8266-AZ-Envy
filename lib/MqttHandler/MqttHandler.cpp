#include <App.hpp>
#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <WifiHandler.hpp>
#include <SensorHandler.hpp>
#include <Util.hpp>
#include "MqttHandler.hpp"

MqttHandler mqttHandler;

static WiFiClient wifiClient;
static PubSubClient client(wifiClient);
static long lastReconnectAttempt = 0;

MqttHandler::MqttHandler() { initialized = false; }

bool MqttHandler::reconnect()
{
  if ((appcfg.mqtt_useauth &&
       client.connect(appcfg.mqtt_clientid, appcfg.mqtt_user,
                      appcfg.mqtt_password)) ||
      (!appcfg.mqtt_useauth && client.connect(appcfg.mqtt_clientid)))
  {
    LOG0("mqtt broker connected\n");
  }
  else
  {
    LOG0("mqtt broker NOT connected\n");
  }

  return client.connected();
}

////////

void MqttHandler::setup()
{
  LOG0("MQTT Setup...\n");
  client.setServer(appcfg.mqtt_host, appcfg.mqtt_port);
  initialized = true;
}

void MqttHandler::handle()
{
  long now = millis();

  if (appcfg.mqtt_enabled && wifiHandler.isReady())
  {
    if (initialized == false)
    {
      setup();
    }

    if (!client.connected())
    {
      if (now - lastReconnectAttempt > 5000)
      {
        LOG0("MQTT try reconnect\n");
        lastReconnectAttempt = now;
        reconnect();
      }
    }
    else
    {
      client.loop();

      if (appcfg.mqtt_sending_interval > 0 &&
          (now - lastPublishTimestamp) > (appcfg.mqtt_sending_interval * 60000))
      {
        sendValue(appcfg.mqtt_topic_temp_c, sensorHandler.tempC);
        sendValue(appcfg.mqtt_topic_temp_f, sensorHandler.tempF);
        sendValue(appcfg.mqtt_topic_humidity, sensorHandler.humidity);
        sendValue(appcfg.mqtt_topic_lpg, sensorHandler.lpg);
        sendValue(appcfg.mqtt_topic_co, sensorHandler.co);
        sendValue(appcfg.mqtt_topic_smoke, sensorHandler.smoke);

        sprintf(buffer,
                "{"
                "\"temp_c\":%.02f,"
                "\"temp_f\":%.02f,"
                "\"humidity\":%.01f,"
                "\"lpg\":%.08f,"
                "\"co\":%.08f,"
                "\"smoke\":%.08f"
                "}\r\n",
                sensorHandler.tempC,
                sensorHandler.tempF,
                sensorHandler.humidity,
                sensorHandler.lpg,
                sensorHandler.co,
                sensorHandler.smoke);
                
        sendValue(appcfg.mqtt_topic_json, buffer);

        lastPublishTimestamp = millis();
      }
    }
  }
}

void MqttHandler::sendValue(const char *topic, const char *value)
{
  if (appcfg.mqtt_enabled && wifiHandler.isReady() && client.connected())
  {
    if (topic != NULL && value != NULL && strlen(topic) > 0 && topic[0] != '-')
    {
      client.clearWriteError();
      client.publish(topic, value);
    }
  }
}

void MqttHandler::sendValue(const char *topic, const float value)
{
  char buffer[32];
  sprintf(buffer, "%0.2f", value);
  sendValue(topic, buffer);
}
