#!/bin/bash

CONTAINER_WORKSPACE=/home/ubuntu/int_brain_ws
CONTAINER_NAME="int_brain_dev"
TARGET=$1

if [ -z "$TARGET" ]; then
    echo "Usage: $0 <target>"
    echo "Available targets: host, sbc"
    exit 1
fi

./stop.sh

# get Git submodules
cd ../../src && \
git submodule update --init --recursive --remote

if [ "$TARGET" == "host" ]; then

    docker run -it -d \
    --privileged \
    --net=host \
    --name $CONTAINER_NAME \
    --volume $PWD/..:$CONTAINER_WORKSPACE \
    --volume /dev:/dev \
    --env="DISPLAY"  \
    --env="QT_X11_NO_MITSHM=1"  \
    --env "TERM=xterm-256color" \
    ghcr.io/eccentricorange/int_brain_host:amd64-dev0.5

elif [ "$TARGET" == "sbc" ]; then

    docker run -it -d \
    --privileged \
    --net=host \
    --name $CONTAINER_NAME \
    --volume $PWD/..:$CONTAINER_WORKSPACE \
    --volume /dev:/dev \
    --env "TERM=xterm-256color" \
    ghcr.io/eccentricOrange/int_brain_sbc:aarch64-dev0.5

else
    echo "Invalid target: $TARGET"  
    echo "Available targets: host, sbc"
    exit 1
fi