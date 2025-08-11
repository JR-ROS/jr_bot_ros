#!/bin/bash

docker build -t intbrain-dev-img:harmonic \
-f $PWD/../.devcontainer/host/Dockerfile \
$PWD/../.devcontainer/host