#!/bin/bash

WORKSPACE_PATH=/workspaces

# source workspace alias
alias wss="source $WORKSPACE_PATH/install/setup.bash && echo \"Sourced workspace\""

# build workspace alias
alias wsb="cd $WORKSPACE_PATH && colcon build --symlink-install && wss"