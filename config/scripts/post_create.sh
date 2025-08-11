#!/bin/bash

# source global ROS environment
source /opt/ros/$ROS_DISTRO/setup.bash

# get Git submodules
cd /workspaces/src && \
git submodule update --init --recursive --remote

# install dependencies
cd /workspaces && \
sudo apt update -y && \
rosdep update && \
rosdep install -y --from-paths src --ignore-src --rosdistro $ROS_DISTRO && \
colcon build --symlink-install

# source script for convenience functions
echo "source /workspaces/src/scripts/convenience_functions.sh" >> ~/.bashrc