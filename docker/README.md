# Build this project in a Docker container

If you like to have always a clean PlatformIO development, Docker is a good choice for this requirement.

## Install Docker

- Docker Desktop (Mac,PC) [https://www.docker.com/products/docker-desktop](https://www.docker.com/products/docker-desktop)

- About Docker CE (Linux also) [https://docs.docker.com/install/](https://docs.docker.com/install/)

## Build PlatformIO Docker image

Simply start `./BUILD_DOCKER_IMAGE.sh` on a Mac or Linux system in this directory.
If you like to rebuild the complete image from scratch start `./FORCE_REBUILD_DOCKER_IMAGE.sh`

## Build the firmware with the `platformio` Docker image

- Edit the `RUN_PLATFORMIO.sh` file and remove all unwanted environments. ( ... -e `<environment name>`)
- Start `./RUN_PLATFORMIO.sh` in this directory.
- Find the builded firmwares in the `<project home>/.pio/build/<environment name>` directories. (I'm now using PIO 4.0)
