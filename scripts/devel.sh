#!/bin/bash

CONTAINER_NAME="intbrain_harmonic"

./stop.sh


# Running on the host machine with the robot connected
docker run -it -d \
--privileged \
--net=host \
--name $CONTAINER_NAME \
--volume $PWD/..:/workspaces/src \
--volume /dev:/dev \
--env="QT_X11_NO_MITSHM=1"  \           
--env="DISPLAY"  \
intbrain-dev-img:harmonic

# Running on the remote machine with the robot connected
# docker run -it -d \
# --privileged \
# --net=host \
# --name $CONTAINER_NAME \
# --volume $PWD/..:/workspaces/src \
# --volume /dev:/dev \
# intbrain-dev-img:harmonic
