FROM ubuntu:20.04

RUN apt-get update && apt-get install -y --no-install-recommends wget unzip git make \
 srecord bc xz-utils gcc python3 curl python3-pip python3-dev build-essential 

RUN pip3 install -U platformio
RUN pio platform install espressif8266@2.6.3 --with-package framework-arduinoespressif8266
RUN pio platform install espressif8266 --with-package framework-arduinoespressif8266

RUN /usr/local/bin/platformio update
RUN /usr/local/bin/platformio upgrade

# Prebuild one project to install the complete environment
RUN git clone https://github.com/thorsten-l/ESP8266-Arduino-Info.git
WORKDIR /ESP8266-Arduino-Info
RUN /usr/local/bin/platformio run
RUN rm -fr /ESP8266-Arduino-Info

RUN mkdir /workdir
WORKDIR /workdir

CMD ["/bin/bash"]
# CMD [ "/usr/local/bin/platformio", "run" ]
