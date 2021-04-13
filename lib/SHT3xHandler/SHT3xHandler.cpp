#include <Arduino.h>
#include <time.h>
#include <App.hpp>
#include "brzo_i2c.h"

#include "SHT3xHandler.hpp"

#if F_CPU == 160000000L
  #define BRZO_I2C_SPEED 1000
#else
  #define BRZO_I2C_SPEED 800
#endif

SHT3xHandler sht3xHandler(SHT3X_I2C_ADDR);

static bool crc8(uint8_t msbData, uint8_t lsbData, uint8_t crcData )
{
  uint8_t crc = 0xFF;
  uint8_t i;

  crc ^= msbData;

  for (i = 0; i < 8; i++)
    crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;

  crc ^= lsbData;

  for (i = 0; i < 8; i++)
    crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;

  return crc == crcData;
}


int SHT3xHandler::updateSHT3xValues()
{
  brzo_i2c_start_transaction( i2cAddress, BRZO_I2C_SPEED );
  brzo_i2c_read( data, 6, false);

  if( brzo_i2c_end_transaction() != 0 )
  {
    return 2;
  }

  // crc check of raw temperature data
  if ( crc8( data[0], data[1], data[2] ) != true )
  {
    return 3;
  }

  // crc check of raw humidity data
  if( crc8( data[3], data[4], data[5] ) != true )
  {
    return 4;
  }

  uint16_t temperatureRaw = (data[0] << 8 ) + data[1];
  cTemp = (float)temperatureRaw;
  cTemp /= 374.4857143f; // 65535 / 175 = 374.4857143f
  cTemp -= 45.0f;

  uint16_t humidityRaw = (data[3] << 8 ) + data[4];
  humidity = (float)humidityRaw;
  humidity /= 655.35f;

	// Convert the data
	// cTemp = ((((data[0] * 256.0) + data[1]) * 175) / 65535.0) - 45;
	fTemp = (cTemp * 1.8) + 32;
	// humidity = ((((data[3] * 256.0) + data[4]) * 100) / 65535.0);

  return 0;
}


SHT3xHandler::SHT3xHandler(uint8_t address)
{
  i2cAddress=address;
  lastValidDataTimestamp = 0l;
  intervalTimestamp = 0l;
  state = STATE_SEND_COMMAND;
  brzo_i2c_setup( SDA, SCL, 0 );
}

void SHT3xHandler::handle()
{
  if ( state == STATE_SEND_COMMAND )
  {
    if (( millis() - intervalTimestamp ) >= UPDATE_INTERVAL )
    {
      intervalTimestamp = millis();
      brzo_i2c_start_transaction( i2cAddress, BRZO_I2C_SPEED );
      data[0] = 0x2C; // select configuration register
      data[1] = 0x06;   // select 16 bit resolution
      brzo_i2c_write( data, 2, false);   // Write two bytes with no repeated start

      if( brzo_i2c_end_transaction() != 0 )
      {
        LOG0( "SHT3x send command error!\n" );
        return;
      }

      state = STATE_READ_DATA;
    }
  }

  if( state == STATE_READ_DATA )
  {
    if (( millis() - intervalTimestamp ) >= 15l )
    {
      intervalTimestamp = millis();

      int error = updateSHT3xValues();

      if ( error == 0 )
      {
        lastValidDataTimestamp = intervalTimestamp;
        // LOG1( "SHT3x Temperature=%.2fC\n", cTemp );
        // LOG1( "SHT3x Humididy=%.2f%%\n", humidity );


        // LOG1( "SHT3x avg counter=%d\n", avgCounter );

        /*if ( avgValuesAvailable )
        {
          LOG1( "SHT3x Avg Temperature=%.2fC\n", avgCTemp );
          LOG1( "SHT3x Avg Humididy=%.2f%%\n", avgHumidity );
        }
        */
      }
      else
      {
        LOG1( "SHT3x read error #%d\n", error );
      }

      state = STATE_SEND_COMMAND;
    }
  }
}
