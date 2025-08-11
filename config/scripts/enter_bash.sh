#!/bin/bash

CONTAINER_NAME="intbrain_harmonic"
TARGET=$1

if [ -z "$TARGET" ]; then
    echo "Usage: $0 <target>"
    echo "Available targets: host, sbc"
    exit 1
fi


if [ "$TARGET" == "host" ]; then

    xhost +local:root
    docker exec -it \
    --env="DISPLAY"  \
    --env="QT_X11_NO_MITSHM=1"  \
    --user="ubuntu" \
    --workdir="/workspaces" \
    $CONTAINER_NAME bash

elif [ "$TARGET" == "sbc" ]; then

    docker exec -it \
    --user="ubuntu" \
    --workdir="/workspaces" \
    $CONTAINER_NAME bash

else
    echo "Invalid target: $TARGET"  
    echo "Available targets: host, sbc"
    exit 1
fi