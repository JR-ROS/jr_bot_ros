#!/bin/bash

CONTAINER_NAME="int_brain_container"
docker stop $CONTAINER_NAME && docker rm $CONTAINER_NAME
xhost -local:root