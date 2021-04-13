#include <App.hpp>
#include <SensorHandler.hpp>
#include "WebPages.h"

static char titleBuffer[100];

void handleRootPage()
{
  sprintf(titleBuffer, APP_NAME " - %s", appcfg.ota_hostname);
  sendHeader(titleBuffer);

  sendPrint("<form class='pure-form pure-form-aligned'><fieldset>");

  sendLegend("Sensor Data");

  int rid=0;
  char valueBuffer[32];

  sprintf( valueBuffer, "%0.02f°C", sensorHandler.tempC );
  sendTextGroupReadOnly( rid++, "Celsius", valueBuffer );

  sprintf( valueBuffer, "%0.02f°F", sensorHandler.tempF );
  sendTextGroupReadOnly( rid++, "Fahrenheit", valueBuffer );

  sprintf( valueBuffer, "%0.01f%%", sensorHandler.humidity );
  sendTextGroupReadOnly( rid++, "Humidity", valueBuffer );

  sprintf( valueBuffer, "%0.08fppm", sensorHandler.lpg );
  sendTextGroupReadOnly( rid++, "LPG", valueBuffer );

  sprintf( valueBuffer, "%0.08fppm", sensorHandler.co );
  sendTextGroupReadOnly( rid++, "CO", valueBuffer );

  sprintf( valueBuffer, "%0.08fppm", sensorHandler.smoke );
  sendTextGroupReadOnly( rid++, "Smoke", valueBuffer );

  sendPrint("</fieldset></form>\n");

  sendPrint( "<script>function getPowerState(){"
    "var tc=document.getElementById('pgid0'),"
        "tf=document.getElementById('pgid1'),"
        "hm=document.getElementById('pgid2');"
        "lp=document.getElementById('pgid3');"
        "co=document.getElementById('pgid4');"
        "sm=document.getElementById('pgid5');"
    "fetch('/api/json').then(resp=>resp.json()).then(function(o){"
      "tc.value=o.temp_c.toFixed(2)+'°C',"
      "tf.value=o.temp_f.toFixed(2)+'°F',"
      "hm.value=o.humidity.toFixed(1)+'%',"
      "lp.value=o.lpg.toFixed(8)+'ppm',"
      "co.value=o.co.toFixed(8)+'ppm',"
      "sm.value=o.smoke.toFixed(8)+'ppm';"
    "})}setInterval(getPowerState,6e4);</script>" );

  sendFooter();
}
