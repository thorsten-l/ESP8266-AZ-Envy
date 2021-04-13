#include <Arduino.h>
#include <FS.h>

#include "MicroJson.hpp"

uJson::uJson(File _file)
{
  file = _file;
  firstEntry = true;
}

void uJson::writeAttributeName(String entryName)
{
  if (firstEntry == false)
  {
    file.write(",\n");
  }
  else
  {
    firstEntry = false;
  }

  file.write("  \"");
  file.write(entryName.c_str());
  file.write("\": ");
}

void uJson::writeHeader() { file.write("{\n"); }

void uJson::writeEntry(String entryName, bool value)
{
  writeAttributeName(entryName);
  file.write((value) ? "true" : "false");
}

void uJson::writeEntry(String entryName, int value)
{
  writeAttributeName(entryName);
  sprintf(buffer, "%d", value);
  file.write((const char *)buffer);
}

void uJson::writeEntry(String entryName, float value)
{
  writeAttributeName(entryName);
  sprintf(buffer, "%f", value);
  file.write((const char *)buffer);
}

void uJson::writeEntry(String entryName, unsigned long value)
{
  writeAttributeName(entryName);
  sprintf(buffer, "%lu", value);
  file.write((const char *)buffer);
}

void uJson::writeEntry(String entryName, const char *value)
{
  writeAttributeName(entryName);
  file.write("\"");
  file.write(value);
  file.write("\"");
}

void uJson::writeFooter() { file.write("\n}\n"); }

bool uJson::findChar(const int c)
{
  return nextNotWhiteSpaceChar() == c;
}

int uJson::nextNotWhiteSpaceChar()
{
  int r;
  while ((r = file.read()) >= 0 && (r == ' ' || r == '\r' || r == '\n' || r == '\t' || r == ',' ));
  return r;
}

bool uJson::readHeader() { return findChar('{'); }

bool uJson::readEntryBoolean(const char *n1, const char *n2, bool *value)
{
  if (strcmp(n1, n2) == 0)
  {
    char nbuffer[16];
    int i = 0;
    int r = nextNotWhiteSpaceChar();
  
    while( r > 0 && i < 16 && r != ' ' && r != '\t' && r != '\r' && r != '\n' && r != ',' )
    {
      nbuffer[i++] = r;
      r = file.read();
    }
  
    if ( r > 0 && i < 16 )
    {
      nbuffer[i] = 0;

      if (strcmp("true", nbuffer) == 0)
      {
        *value = true;
        return false;
      }
      if (strcmp("false", nbuffer) == 0)
      {
        *value = false;
        return false;
      }
    }
  }
  else
  {
    return false;
  }

  return true;
}

bool uJson::readEntryInteger(const char *n1, const char *n2, int *value)
{
  if (strcmp(n1, n2) == 0)
  {
    char nbuffer[32];
    int i = 0;
    int r = nextNotWhiteSpaceChar();
  
    while( r > 0 && i < 32 && r != ' ' && r != '\t' && r != '\r' && r != '\n' && r != ',' )
    {
      nbuffer[i++] = r;
      r = file.read();
    }
  
    if ( r > 0 && i < 32 )
    {
      nbuffer[i] = 0;
      if ( sscanf( nbuffer, "%d", value ) == 1 )
      {
        return false;
      }
    }
  }
  else
  {
    return false;
  }
 
  return true;
}

bool uJson::readEntryFloat(const char *n1, const char *n2, float *value)
{
  if (strcmp(n1, n2) == 0)
  {
    char nbuffer[32];
    int i = 0;
    int r = nextNotWhiteSpaceChar();
  
    while( r > 0 && i < 32 && r != ' ' && r != '\t' && r != '\r' && r != '\n' && r != ',' )
    {
      nbuffer[i++] = r;
      r = file.read();
    }
  
    if ( r > 0 && i < 32 )
    {
      nbuffer[i] = 0;
      if ( sscanf( nbuffer, "%f", value ) == 1 )
      {
        return false;
      }
    }
  }
  else
  {
    return false;
  }
 
  return true;
}

bool uJson::readEntryULong(const char *n1, const char *n2,
                           unsigned long *value)
{
  if (strcmp(n1, n2) == 0)
  {
    char nbuffer[32];
    int i = 0;
    int r = nextNotWhiteSpaceChar();
  
    while( r > 0 && i < 32 && r != ' ' && r != '\t' && r != '\r' && r != '\n' && r != ',' )
    {
      nbuffer[i++] = r;
      r = file.read();
    }
  
    if ( r > 0 && i < 32 )
    {
      nbuffer[i] = 0;
      if ( sscanf( nbuffer, "%lu", value ) == 1 )
      {
        return false;
      }
    }
  }
  else
  {
    return false;
  }

  return true;
}

bool uJson::readEntryChars(const char *n1, const char *n2, char *value)
{
  if (strcmp(n1, n2) == 0 && findChar('"'))
  {
    int i = 0;
    int r;
    while ((r = file.read()) >= 0 && r != '"' && i < 64)
    {
      value[i++] = r;
    }
    if (r == '"' && i < 64)
    {
      value[i] = 0;
      return false;
    }
  }
  else
  {
    return false;
  }

  return true;
}

bool uJson::readAttributeName(char *attributeName)
{
  bool attributeNameFound = false;
  if (findChar('"') == true)
  {
    int i = 0;
    int r;
    while ((r = file.read()) >= 0 && r != '"' && i < 128)
    {
      attributeName[i++] = r;
    }
    if (r == '"' && i < 128 && findChar(':'))
    {
      attributeName[i] = 0;
      attributeNameFound = true;
    }
  }

  return attributeNameFound;
}
