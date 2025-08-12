#!/bin/bash

CONTAINER_NAME="int_brain_dev"
docker stop $CONTAINER_NAME && docker rm $CONTAINER_NAME
xhost -local:root