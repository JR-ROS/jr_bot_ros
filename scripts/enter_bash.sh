#!/bin/bash

# Running on the host machine
xhost +local:root
docker exec -it \
--env="DISPLAY"  \
--env="QT_X11_NO_MITSHM=1"  \
--user="ubuntu" \
--workdir="/workspaces" \
intbrain_harmonic bash

# Running on the remote machine
# docker exec -it \
# --user="ubuntu" \
# --workdir="/workspaces" \
# intbrain_harmonic bash