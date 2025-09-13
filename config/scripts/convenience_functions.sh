#!/bin/bash

# source workspace alias
alias wss="source $WORKSPACE/install/setup.bash && echo \"Sourced workspace\""

# build workspace alias
alias wsb="cd $WORKSPACE && colcon build --symlink-install && wss"

# god forsaken rosdep install alias
alias rdi="sudo rosdep install -y --from-paths $WORKSPACE/src --ignore-src --rosdistro $ROS_DISTRO"