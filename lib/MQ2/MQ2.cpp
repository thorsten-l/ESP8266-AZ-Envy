#include <Arduino.h>
#include "MQ2.h"
#include <App.hpp>
#include <TelnetStream.h>

MQ2::MQ2(int pin)
{
	_pin = pin;
	Ro = -1.0;
}

void MQ2::begin()
{
	Ro = MQCalibration();
	handlerStatus = MQ2_HANDLER_STATUS_FINISHED;
	handlerIteration = 0;
	handlerTimestamp = 0;
	Serial.print("Ro: ");
	Serial.print(Ro);
	Serial.println(" kohm");
}

void MQ2::close()
{
	Ro = -1.0;
	values[0] = 0.0;
	values[1] = 0.0;
	values[2] = 0.0;
}

bool MQ2::checkCalibration()
{
	if (Ro < 0.0)
	{
		Serial.println("Device not calibrated, call MQ2::begin before reading any value.");
		return false;
	}

	return true;
}

float *MQ2::read(bool print)
{
	if (!checkCalibration())
		return NULL;

	values[0] = MQGetPercentage(LPGCurve);
	values[1] = MQGetPercentage(COCurve);
	values[2] = MQGetPercentage(SmokeCurve);

	lastReadTime = millis();

	if (print)
	{
		Serial.print(lastReadTime);
		Serial.print("ms - LPG:");
		Serial.print(values[0], 5);
		Serial.print("ppm\t");
		Serial.print("CO:");
		Serial.print(values[1], 5);
		Serial.print("ppm\t");
		Serial.print("SMOKE:");
		Serial.print(values[2], 5);
		Serial.print("ppm\n");
	}

	return values;
}

float MQ2::readLPG()
{
	if (!checkCalibration())
		return 0.0;

	if (millis() - lastReadTime >= READ_DELAY && values[0] > 0)
		return values[0];
	else
		return (values[0] = MQGetPercentage(LPGCurve));
}

float MQ2::readCO()
{
	if (!checkCalibration())
		return 0.0;

	if (millis() - lastReadTime >= READ_DELAY && values[1] > 0)
		return values[1];
	else
		return (values[1] = MQGetPercentage(COCurve));
}

float MQ2::readSmoke()
{
	if (!checkCalibration())
		return 0.0;

	if (millis() - lastReadTime >= READ_DELAY && values[2] > 0)
		return values[2];
	else
		return (values[2] = MQGetPercentage(SmokeCurve));
}

float MQ2::MQResistanceCalculation(int raw_adc)
{
	float flt_adc = (float)raw_adc;
	return RL_VALUE * (1023.0 - flt_adc) / flt_adc;
}

float MQ2::MQCalibration()
{
	float val = 0.0;

	// take multiple samples
	for (int i = 0; i < CALIBARAION_SAMPLE_TIMES; i++)
	{
		val += MQResistanceCalculation(analogRead(_pin));
		delay(CALIBRATION_SAMPLE_INTERVAL);
	}

	//calculate the average value
	val = val / ((float)CALIBARAION_SAMPLE_TIMES);

	//divided by RO_CLEAN_AIR_FACTOR yields the Ro according to the chart in the datasheet
	val = val / RO_CLEAN_AIR_FACTOR;

	return val;
}

float MQ2::MQRead()
{
	float rs = 0.0;

	for (int i = 0; i < READ_SAMPLE_TIMES; i++)
	{
		rs += MQResistanceCalculation(analogRead(_pin));
		delay(READ_SAMPLE_INTERVAL);
	}

	return rs / ((float)READ_SAMPLE_TIMES);
}

float MQ2::MQGetPercentage(float *pcurve)
{
	float rs_ro_ratio = MQRead() / Ro;
	return pow(10.0, ((log(rs_ro_ratio) - pcurve[1]) / pcurve[2]) + pcurve[0]);
}

float *MQ2::handle()
{
	static int toggle;

	if (handlerStatus > MQ2_HANDLER_STATUS_FINISHED)
	{
		switch (handlerStatus)
		{

		case MQ2_HANDLER_STATUS_BEGIN:
			tlog("MQ2_HANDLER_STATUS_BEGIN\n");
			if (checkCalibration() == true)
			{
				toggle = 0;
				handlerIteration = 0;
				handlerRs = 0.0;
				handlerStatus = MQ2_HANDLER_STATUS_READING;
			}
			else
			{
				tlog("ERROR: Calibration\n");
			}
			break;

		case MQ2_HANDLER_STATUS_READING:
		{
			if (toggle == 0)
			{
				handlerRs += MQResistanceCalculation(analogRead(_pin));
				handlerIteration++;

				if (handlerIteration >= READ_SAMPLE_TIMES)
				{
					handlerRs /= ((float)READ_SAMPLE_TIMES);
					handlerStatus = MQ2_HANDLER_STATUS_READ_END;
				}
			}
			toggle = 1 - toggle; // read every second iteration ~ 50-60ms
		}
		break;

		case MQ2_HANDLER_STATUS_READ_END:
		{
			tlogf("MQ2_HANDLER_STATUS_READ_END rs=%f\n", handlerRs);
			float rs_ro_ratio = handlerRs / Ro;
			values[0] = pow(10.0, ((log(rs_ro_ratio) - LPGCurve[1]) / LPGCurve[2]) + LPGCurve[0]);
			values[1] = pow(10.0, ((log(rs_ro_ratio) - COCurve[1]) / COCurve[2]) + COCurve[0]);
			values[2] = pow(10.0, ((log(rs_ro_ratio) - SmokeCurve[1]) / SmokeCurve[2]) + SmokeCurve[0]);
			handlerStatus = MQ2_HANDLER_STATUS_FINISHED;
		}
		break;
		}
	}
	else if ((millis() - handlerTimestamp) >= MQ2_HANDLER_INERVAL)
	{
		handlerStatus = MQ2_HANDLER_STATUS_BEGIN;
		handlerTimestamp = millis();
	}

	return values;
}

float *MQ2::update()
{
	Ro = MQCalibration();
	tlogf("Ro: %f kOhm\n", Ro);
	delay(100);
	float rs_ro_ratio = MQRead() / Ro;
	values[0] = pow(10.0, ((log(rs_ro_ratio) - LPGCurve[1]) / LPGCurve[2]) + LPGCurve[0]);
	values[1] = pow(10.0, ((log(rs_ro_ratio) - COCurve[1]) / COCurve[2]) + COCurve[0]);
	values[2] = pow(10.0, ((log(rs_ro_ratio) - SmokeCurve[1]) / SmokeCurve[2]) + SmokeCurve[0]);
	return values;
}