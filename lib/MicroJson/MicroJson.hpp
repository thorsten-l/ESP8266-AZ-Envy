#ifndef __MICRO_JSON_HPP__
#define __MICRO_JSON_HPP__

#include <Arduino.h>
#include <FS.h>

class uJson
{
private:
  File file;
  bool firstEntry;
  char buffer[128];

  bool findChar( const int c );
  int nextNotWhiteSpaceChar();
  void writeAttributeName( String entryName );

public:
  uJson( File _file );

  void writeHeader();
  void writeEntry( String entryName, bool value );
  void writeEntry( String entryName, int value );
  void writeEntry( String entryName, float value );
  void writeEntry( String entryName, unsigned long value );
  void writeEntry( String entryName, const char *value );
  void writeFooter();

  bool readHeader();
  bool readAttributeName( char *attributeName );
  bool readEntryBoolean( const char* n1, const char* n2, bool *value );
  bool readEntryInteger( const char* n1, const char* n2, int *value );
  bool readEntryULong( const char* n1, const char* n2, unsigned long *value );
  bool readEntryFloat( const char* n1, const char* n2, float *value );
  bool readEntryChars( const char* n1, const char* n2, char *value );
  bool readFooter();
};

#endif
