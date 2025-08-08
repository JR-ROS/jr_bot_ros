#!/bin/bash

CONTAINER_NAME="intbrain_harmonic"

./stop.sh

docker run -it -d --privileged --net=host --cpus="3" \
--name $CONTAINER_NAME \
--env="QT_X11_NO_MITSHM=1"  \
--env="DISPLAY"  \
-v $PWD/..:/workspaces/src \
-v /dev:/dev \
intbrain-dev-img:harmonic