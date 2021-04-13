#include <App.hpp>
#include "SensorHandler.hpp"
#include <MQ2.h>
#include <SHT3xHandler.hpp>
#include <TelnetStream.h>

SensorHandler sensorHandler;
MQ2 mq2(MQ2_PIN);

SensorHandler::SensorHandler()
{
  timestamp = 0;
}

void SensorHandler::setup()
{
  LOG0("Sensor setup... ");
  pinMode(MQ2_PIN, INPUT);
  mq2.begin();
  LOG0("done.\n");
}

void SensorHandler::handle()
{
  sht3xHandler.handle();
  float *values = mq2.handle();

  if ((millis() - timestamp) >= 30000)
  {
    tempC = sht3xHandler.cTemp + appcfg.offset_c;
    tempF = sht3xHandler.fTemp + appcfg.offset_f;
    humidity = sht3xHandler.humidity;

    lpg = values[0] * appcfg.factor_lpg;
    co = values[1] * appcfg.factor_co;
    smoke = values[2] * appcfg.factor_smoke;

    tlogf("tempC=%f, tempF=%f, humidity=%f, lpg=%f, co=%f, smoke=%f\n",
         tempC, tempF, humidity, lpg, co, smoke);

    timestamp = millis();
  }
}

void SensorHandler::calibrate()
{
  tlog("Recalibrating sensors\n");
  mq2.update();
  tlogf("tempC=%f, tempF=%f, humidity=%f, lpg=%f, co=%f, smoke=%f\n",
       tempC, tempF, humidity, lpg, co, smoke);
}