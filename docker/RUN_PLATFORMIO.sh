#!/bin/sh
cd ..
docker run -it --rm \
  -v `pwd`:/workdir \
  --name platformio platformio \
  /usr/local/bin/platformio run -e az_envy
  