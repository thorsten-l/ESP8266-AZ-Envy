#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <App.hpp>

#define MAX_MESSAGE_LENGTH 200
#define MESSAGE_BUFFER_LINES 11
#define BUFFER_LENGTH 4096
#define BUFFER2_LENGTH 256

extern char messageBuffer[];
extern int messageStartIndex;
extern int messageEndIndex;

extern int getBootDevice(void);
extern void alterPin(int pin);
extern void showChipInfo();
extern void fillBuffer(const char *message);

extern void sendHtmlTemplate(const char *htmlTemplate, const char *(*setupProcessor)(const char *));

extern char buffer[];
extern char buffer2[];

#endif
