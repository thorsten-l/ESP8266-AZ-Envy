#include <App.hpp>

const char MAINTENANCE_STYLE[] PROGMEM =
    "<style>\n"
        ".fuContainer { "
          "margin: auto; position: absolute; top: 0; left: 0; bottom: 0; \n"
           "right: 0; width: 70%; text-align: center; display: none; align-items: center; \n"
           "justify-content: center; }\n"
        ".fuMessage { "
           "padding: 1em; outline: 1px solid black; background: rgb(178, 240, 187);\n"
           "text-align: center; }\n"
    "</style>\n";

const char MAINTENANCE_HTML_TEMPLATE[] PROGMEM =

            "<div id='uploadMessage' class='fuContainer'>\n"
                "<div class='fuMessage'>\n"
                    "<h3>Firmware upload in progress...</h3>\n"
                "</div>\n"
            "</div>\n"

           "<div class='accordion'>Backup & Restore Configuration</div>\n"
            "<div class='panel sub-panel'>\n"

                "<form class='pure-form pure-form-aligned' method='GET' action='/config-backup' >\n"
                    "<fieldset>\n"
                        "<legend>Backup Configuration</legend>\n"
                        "<p><input class='pure-button pure-button-primary' type='submit' value='Download'></p>\n"
                    "</fieldset>\n"
                "</form>\n"

                "<form class='pure-form pure-form-aligned' method='POST' action='/config-restore' enctype='multipart/form-data'>\n"
                    "<fieldset>\n"
                        "<legend>Restore Configuration</legend>\n"
                        "<div>\n"
                            "<input class='inputfile' id='ucid34' type='file' name='upload'>\n"
                            "<label for='ucid34'><svg xmlns='http://www.w3.org/2000/svg' width='24' height='20' viewBox='0 0 24 20'><path d='M10 0l-5.2 4.9h3.3v5.1h3.8v-5.1h3.3l-5.2-4.9zm9.3 11.5l-3.2-2.1h-2l3.4 2.6h-3.5c-.1 0-.2.1-.2.1l-.8 2.3h-6l-.8-2.2c-.1-.1-.1-.2-.2-.2h-3.6l3.4-2.6h-2l-3.2 2.1c-.4.3-.7 1-.6 1.5l.6 3.1c.1.5.7.9 1.2.9h16.3c.6 0 1.1-.4 1.3-.9l.6-3.1c.1-.5-.2-1.2-.7-1.5z'/></svg> <span>Choose a <b><tt>config.json</tt></b> file&hellip;</span></label>\n"
                        "</div>\n"
                        "<p><input class='pure-button pure-button-primary' type='submit' value='Upload'></p>\n"
                    "</fieldset>\n"
                "</form>\n"
            "</div>\n"
            "</div>\n"

           "<div class='accordion'>Firmware</div>\n"
            "<div class='panel sub-panel'>\n"

                "<form class='pure-form pure-form-aligned' method='POST' action='/update-firmware' enctype='multipart/form-data'>\n"
                    "<fieldset>\n"
                        "<legend>Upload new firmware</legend>\n"
                        "<div>\n"
                            "<input class='inputfile' id='pgid34' type='file' name='update'>\n"
                            "<label for='pgid34'><svg xmlns='http://www.w3.org/2000/svg' width='24' height='20' viewBox='0 0 24 20'><path d='M10 0l-5.2 4.9h3.3v5.1h3.8v-5.1h3.3l-5.2-4.9zm9.3 11.5l-3.2-2.1h-2l3.4 2.6h-3.5c-.1 0-.2.1-.2.1l-.8 2.3h-6l-.8-2.2c-.1-.1-.1-.2-.2-.2h-3.6l3.4-2.6h-2l-3.2 2.1c-.4.3-.7 1-.6 1.5l.6 3.1c.1.5.7.9 1.2.9h16.3c.6 0 1.1-.4 1.3-.9l.6-3.1c.1-.5-.2-1.2-.7-1.5z'/></svg> <span>Choose a <b><tt>firmware-%pioenv_name%.bin</tt></b> firmware file&hellip;</span></label>\n"
                        "</div>\n"
                        "<p><input class='pure-button pure-button-primary' type='submit' value='Upload' onclick=\"document.getElementById('uploadMessage').style.display = 'flex';\"></p>\n"
                    "</fieldset>\n"
                "</form>\n"

                "<form class='pure-form pure-form-aligned' method='POST' action='/reset-firmware' >\n"
                    "<fieldset>\n"
                        "<legend>Firmware Reset</legend>\n"
                        "<div>\n"
                            "<div class='pure-control-group'><label for='pgid35'>Really reset to system defaults?</label><select id='pgid35' name='doreset'><option selected='' value='false'>No</option><option value='true'>Yes</option></select></div>\n"
                        "</div>\n"
                        "<p><input class='pure-button pure-button-primary' type='submit' value='Reset'></p>\n"
                    "</fieldset>\n"
                "</form>\n"

            "</div>\n"

            "<div class='accordion'>System</div>\n"
            "<div class='panel sub-panel'>\n"
                "<form class='pure-form pure-form-aligned' method='POST' action='/system-restart' >\n"
                    "<fieldset>\n"
                        "<legend>Restart system</legend>\n"
                        "<p><input class='pure-button pure-button-primary' type='submit' value='Restart'></p>\n"
                    "</fieldset>\n"
                "</form>\n"
            "</div>\n"

        "<p>&nbsp;</p></div>\n"

  "<script type='text/javascript'>\n"
    "var input = document.getElementById('pgid34'),label = input.nextElementSibling,labelVal = label.innerHTML;\n"
    "input.addEventListener('change', function(e) {var l = e.target.value.split('\\\\').pop();\n"
    "l ? label.querySelector('span').innerHTML = l : label.innerHTML = labelVal});\n"
 
    "var input2 = document.getElementById('ucid34'),label2 = input2.nextElementSibling,labelVal2 = label2.innerHTML;\n"
    "input2.addEventListener('change', function(e) {var l2 = e.target.value.split('\\\\').pop();\n"
    "l2 ? label2.querySelector('span').innerHTML = l2 : label2.innerHTML = labelVal2});\n"
  "</script>\n"
  ;