#!/bin/bash

CONTAINER_NAME="intbrain_harmonic"
IMAGE_NAME="intbrain-dev-img:harmonic"
TARGET=$1

if [ -z "$TARGET" ]; then
    echo "Usage: $0 <target>"
    echo "Available targets: host, sbc"
    exit 1
fi

./stop.sh

if [ "$TARGET" == "host" ]; then

    docker run -it -d \
    --privileged \
    --net=host \
    --name $CONTAINER_NAME \
    --volume $PWD/..:/workspaces/src \
    --volume /dev:/dev \
    --env="DISPLAY"  \
    --env="QT_X11_NO_MITSHM=1"  \
    $IMAGE_NAME

elif [ "$TARGET" == "sbc" ]; then

    docker run -it -d \
    --privileged \
    --net=host \
    --name $CONTAINER_NAME \
    --volume $PWD/..:/workspaces/src \
    --volume /dev:/dev \
    $IMAGE_NAME

else
    echo "Invalid target: $TARGET"  
    echo "Available targets: host, sbc"
    exit 1
fi