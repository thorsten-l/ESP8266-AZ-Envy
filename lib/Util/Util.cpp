#include "Util.hpp"
#include <Arduino.h>

extern void sendPrint(const char *message);

char buffer[BUFFER_LENGTH+1];
char buffer2[BUFFER2_LENGTH+1];

#define MAX_MESSAGE_LENGTH 200
#define MESSAGE_BUFFER_LINES 11
char messageBuffer[(MAX_MESSAGE_LENGTH + 1) * MESSAGE_BUFFER_LINES];
int messageStartIndex = 0;
int messageEndIndex = 0;

/*
Find the description about the boot device code here:
https://www.sigmdel.ca/michel/program/esp8266/arduino/watchdogs2_en.html
*/
int getBootDevice(void)
{
  int bootmode = 0;
  asm("movi %0, 0x60000200\n\t"
      "l32i %0, %0, 0x118\n\t"
      : "+r"(bootmode) /* Output */
      :                /* Inputs (none) */
      : "memory"       /* Clobbered */
  );
  return ((bootmode >> 0x10) & 0x7);
}

void alterPin(int pin)
{
  digitalWrite(pin, 1 ^ digitalRead(pin));
}

void showChipInfo()
{
  Serial.println("-- CHIPINFO --");
  Serial.printf("Chip Id = %08X\n", ESP.getChipId());

  Serial.printf("CPU Frequency = %dMHz\n", ESP.getCpuFreqMHz());

  uint32_t realSize = ESP.getFlashChipRealSize();
  uint32_t ideSize = ESP.getFlashChipSize();
  FlashMode_t ideMode = ESP.getFlashChipMode();

  Serial.printf("\nFlash real id:   %08X\n", ESP.getFlashChipId());
  Serial.printf("Flash real size: %u\n", realSize);
  Serial.printf("Flash ide  size: %u\n", ideSize);
  Serial.printf("Flash chip speed: %u\n", ESP.getFlashChipSpeed());
  Serial.printf("Flash ide mode:  %s\n", (ideMode == FM_QIO    ? "QIO"
                                          : ideMode == FM_QOUT ? "QOUT"
                                          : ideMode == FM_DIO  ? "DIO"
                                          : ideMode == FM_DOUT ? "DOUT"
                                                               : "UNKNOWN"));

  if (ideSize != realSize)
  {
    Serial.println("Flash Chip configuration wrong!\n");
  }
  else
  {
    Serial.println("Flash Chip configuration ok.\n");
  }

  Serial.printf("Free Heap         : %u\n", ESP.getFreeHeap());
  Serial.printf("Sketch Size       : %u\n", ESP.getSketchSize());
  Serial.printf("Free Sketch Space : %u\n", ESP.getFreeSketchSpace());

  Serial.println();
}

void fillBuffer(const char *message)
{
  strncpy(buffer, message, BUFFER_LENGTH - 1);
  buffer[BUFFER_LENGTH - 1] = 0;
}

void sendHtmlTemplate(const char *htmlTemplate, const char *(*setupProcessor)(const char *))
{
  unsigned int s = 0;
  unsigned int d = 0;
  char c;
  int state = 0;

  while ((c = pgm_read_byte(htmlTemplate + (s++))) > 0)
  {
    switch (state)
    {
    case 0:
    {
      if (c == '%')
      {
        sendPrint(buffer);
        d = 0;
        state = 1;
        break;
      }

      buffer[d++] = c;
      buffer[d] = 0;

      if (c == '\n')
      {
        sendPrint(buffer);
        d = 0;
      }
      break;
    }

    case 1:
    {
      if (c == '%')
      {
        const char *val = (*setupProcessor)(buffer);

        if (val != nullptr && strlen(val) > 0)
        {
          sendPrint(val);
        }

        d = 0;
        state = 0;
        break;
      }

      buffer[d++] = c;
      buffer[d] = 0;
      break;
    }
    }
  }

  if (d > 0)
  {
    sendPrint(buffer);
  }
}
