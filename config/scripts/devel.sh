#!/bin/bash

CONTAINER_WORKSPACE=/home/ubuntu/jr_bot_ws
CONTAINER_NAME="jr_bot_container"
TARGET=$1
IMAGE_TAG="v1.1.1"

if [ -z "$TARGET" ]; then
    echo "Usage: $0 <target>"
    echo "Available targets: host, sbc"
    exit 1
fi

config/scripts/stop.sh

# get Git submodules
git submodule update --init --recursive

if [ "$TARGET" == "host" ]; then

    docker run -it -d \
    --net=host \
    --name $CONTAINER_NAME \
    --privileged \
    --volume $PWD:$CONTAINER_WORKSPACE/src \
    --volume /dev:/dev \
    --env="DISPLAY"  \
    --env="QT_X11_NO_MITSHM=1"  \
    --env "TERM=xterm-256color" \
    --user ubuntu \
    ghcr.io/jr-ros/jr_bot_host:amd64-$IMAGE_TAG

elif [ "$TARGET" == "sbc" ]; then

    docker run -it -d \
    --net=host \
    --name $CONTAINER_NAME \
    --privileged \
    --volume $PWD:$CONTAINER_WORKSPACE/src \
    --volume /dev:/dev \
    --env "TERM=xterm-256color" \
    --user ubuntu \
    ghcr.io/jr-ros/jr_bot_sbc:aarch64-$IMAGE_TAG

else
    echo "Invalid target: $TARGET"  
    echo "Available targets: host, sbc"
    exit 1
fi

docker exec -it \
$CONTAINER_NAME \
$CONTAINER_WORKSPACE/src/config/scripts/post_create.sh