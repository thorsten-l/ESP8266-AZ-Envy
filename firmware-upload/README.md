# Firmware Binary
If you do not wish to compile this firmware by your own, you can use the
precompiled firmware from the [github releases](https://github.com/thorsten-l/ESP8266-AZ-Envy/releases).

---

## Initial Firmware Upload via USB

If you upload this Firmware for the very first time on your AZ-Envy Device you have to do it via USB.

### Flash-Mode for USB-Upload
To upload a new Firmware via USB you have to put your AZ-Envy in `flash-mode`.

1. Press and hold the `Flash-Button`.
2. Press and release the `Reset-Button`.
3. Release the `Flash-Button`.

Now your AZ-Envy is ready for firmware upload.

### USB_UPLOAD script
A sample script to upload the firmware via USB connection.

**Change the USB device in the script
to your device path.**

### esptool
The [esptool](https://github.com/espressif/esptool) is needed to upload the firmware over USB.

I'm dealing with the `esptool` comes with [PlatformIO](https://platformio.org/).

---

# Firmware Upgrade

If this firmware is already running on your AZ-Envy, this easiest way to up upgrade your Firmware is via a Web-Browser. Go into the `Maintanance` menu and upload the new firmware from [github releases](https://github.com/thorsten-l/ESP8266-AZ-Envy/releases)

## References
- [PlatformIO](https://platformio.org/)
- [esptool](https://github.com/espressif/esptool)
- [espota.py](https://github.com/esp8266/Arduino/blob/master/tools/espota.py)
