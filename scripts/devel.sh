#!/bin/bash

CONTAINER_NAME="intbrain_harmonic"

./stop.sh


# Running on the host machine with the robot connected
# docker run -it -d \
# --privileged \
# --net=host \
# --name $CONTAINER_NAME \
# --volume $PWD/..:/workspaces/src \
# --env="QT_X11_NO_MITSHM=1"  \           
# --env="DISPLAY"  \
# --device=/dev/int-brain-xxxxx \     # UPDATE THIS WITH THE ACTUAL DEVICE NAME
# intbrain-dev-img:harmonic


# Running on the host machine without the robot connected
docker run -it -d \
--privileged \
--net=host \
--name $CONTAINER_NAME \
--volume $PWD/..:/workspaces/src \
--env="QT_X11_NO_MITSHM=1"  \           
--env="DISPLAY"  \
intbrain-dev-img:harmonic

# Running on the remote machine with the robot connected
# docker run -it -d \
# --privileged \
# --net=host \
# --name $CONTAINER_NAME \
# --volume $PWD/..:/workspaces/src \
# --device=/dev/int-brain-xxxxx \     # UPDATE THIS WITH THE ACTUAL DEVICE NAME
# intbrain-dev-img:harmonic

# Running on the remote machine without the robot connected
# docker run -it -d \
# --privileged \
# --net=host \
# --name $CONTAINER_NAME \
# --volume $PWD/..:/workspaces/src \
# intbrain-dev-img:harmonic