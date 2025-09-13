#!/bin/bash

# take ownership of folders
# sudo chown -R ubuntu:ubuntu $WORKSPACE

# source global ROS environment
source /opt/ros/$ROS_DISTRO/setup.bash

# install dependencies
cd $WORKSPACE && \
sudo apt update -y && \
rosdep update && \
rosdep install -y --from-paths src --ignore-src --rosdistro $ROS_DISTRO && \
colcon build --symlink-install

# source script for convenience functions
echo "source $WORKSPACE/src/config/scripts/convenience_functions.sh" >> ~/.bashrc