ARG ROS_DISTRO=jazzy

## get base image
FROM ros:$ROS_DISTRO-ros-base

## config args
ARG USERNAME=ubuntu

## make ubuntu a sudo user
RUN echo "ubuntu ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/ubuntu \
    && chmod 0440 /etc/sudoers.d/ubuntu

## update system
RUN apt-get clean \
    && rm -rf /var/lib/apt/lists/* \
    && apt-get update -y

## install basic tools
RUN apt-get install --no-install-recommends -y \
    vim \
    bat \
    python3-pip \
    python3-venv \
    net-tools \
    tree

## install ROS packages
# Install Gz sim
RUN apt-get install --no-install-recommends -y \
    ros-$ROS_DISTRO-ros-gz-sim \
    ros-$ROS_DISTRO-ros-gz-interfaces \
    ros-$ROS_DISTRO-ros-gz-bridge

# Install ros2 control
RUN apt-get install --no-install-recommends -y \
    ros-$ROS_DISTRO-ros2-control \
    ros-$ROS_DISTRO-ros2-controllers \
    ros-$ROS_DISTRO-rqt-controller-manager \
    ros-$ROS_DISTRO-gz-ros2-control

# Install sensor fusion
RUN apt-get install --no-install-recommends -y \
    ros-$ROS_DISTRO-robot-localization

# Install teleop packages
RUN apt-get install --no-install-recommends -y \
    ros-$ROS_DISTRO-teleop-twist-joy \
    ros-$ROS_DISTRO-teleop-twist-keyboard \
    ros-$ROS_DISTRO-joy

# Install plotjuggler
RUN apt-get install --no-install-recommends -y \
    ros-$ROS_DISTRO-rviz2 \
    ros-$ROS_DISTRO-rviz-imu-plugin \
    ros-$ROS_DISTRO-xacro \
    ros-$ROS_DISTRO-joint-state-publisher-gui \
    ros-$ROS_DISTRO-plotjuggler-ros

## set default shell to bash 
SHELL ["/bin/bash", "-c"]

## set variables
ENV ROS_DISTRO=$ROS_DISTRO
ENV ROS_DOMAIN_ID=42

## create workspace
RUN mkdir -p /workspaces/src

# convenience scripts
RUN echo "source /opt/ros/$ROS_DISTRO/setup.bash" >> /home/$USERNAME/.bashrc
RUN echo "source /workspaces/install/setup.bash && echo \"Sourced workspace\"" >> /home/$USERNAME/.bashrc

## add to groups
RUN usermod -aG dialout $USERNAME
RUN usermod -aG video $USERNAME

## set username
USER $USERNAME