# ESP8266-AZ-Envy

## Firmware

- AZ-Delivery, AZ-Envy, [https://www.az-delivery.de/products/az-envy](https://www.az-delivery.de/products/az-envy)

![homepage](./images/img01.png)

## 3D files for a case
- [3d-print](./3d-print/)

## Compile

This is a [PlatformIO](https://platformio.org/) project.

## Firmware Reset

1. Power on the AZ-Envy (plug in)
2. Press down and hold the Flash-Button during the first 5s
3. Release Flash-Button when the WiFi LED starts rapidly fast blinking
4. The firmware reset ends after about 30s indicated by a permanent WiFi LED on.

## Recalibrate the MQ2 gas sensor

- Click Flash-Button

## Default Settings

After a firmware reset the following values are set.

|                            | value                 | description                           |
| -------------------------- | --------------------- | ------------------------------------- |
| Admin password             | admin                 | Setup admin password                  |
| WiFi Mode                  | AP                    | Accesspoint- or Station-Mode          |
| WiFi SSID                  | az-envy-`esp id` | SSID in accesspoint mode              |
| WiFi Password              | 12345678              | Default WiFi password                 |
| Network Mode               | DHCP                  | Network mode STATIC or DHCP \*1       |
| Network IP-Address         | 192.168.192.1         | device ip address \*2                 |
| Network Mask               | 255.255.255.0         | Subnet mask \*2                       |
| Network Gateway            | 192.168.192.1         | Default gateway \*2                   |
| Network DNS                | 192.168.192.1         | DNS server address \*2                |
| OTA Hostname               | az-envy-`esp id`         | *O*ver *T*he *A*ir hostname           |
| OTA Password               | otapass               | OTA firmware upload password          |

\*1 In WiFi Station-Mode only

\*2 will be overwritten from DHCP-Response


## References

- [PlatformIO](https://platformio.org/)
- [Arduino core for ESP8266 WiFi chip](https://github.com/esp8266/Arduino)
- [Arduino-pubsubclient - A client library for the ESP8266 that provides support for MQTT](https://github.com/knolleary/pubsubclient)
- [Pure.CSS - A nice CSS, as GZIP it is less than 4k so it is useful for embedded devices](https://purecss.io/)
