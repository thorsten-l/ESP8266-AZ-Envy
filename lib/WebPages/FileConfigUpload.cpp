#include <App.hpp>
#include <FS.h>
#include <LittleFS.h>
#include "WebPages.h"

static File fsUploadFile;

void handleConfigFileUpload()
{
  HTTPUpload &upload = server.upload();

  if (upload.status == UPLOAD_FILE_START)
  {
    String filename = upload.filename;

    if (!filename.startsWith("/"))
      filename = "/" + filename;

    Serial.print("handleFileUpload Name: ");
    Serial.println(filename);

    LittleFS.begin();
    fsUploadFile = LittleFS.open(APP_CONFIG_FILE_JSON, "w");
    filename = String();
  }
  else if (upload.status == UPLOAD_FILE_WRITE)
  {
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file
  }
  else if (upload.status == UPLOAD_FILE_END)
  {
    if (fsUploadFile)
    {
      fsUploadFile.close();
      Serial.print("handleFileUpload Size: ");
      Serial.println(upload.totalSize);
    }
    else
    {
      server.send(500, "text/plain", "500: couldn't upload file");
    }
  }
}
