ARG ROS_DISTRO=jazzy

## get base image
FROM ros:$ROS_DISTRO-ros-base

## config args
ARG USERNAME=ubuntu
ARG WORKSPACE=/home/$USERNAME/int_brain_ws

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

## set default shell to bash 
SHELL ["/bin/bash", "-c"]

## add to groups
RUN usermod -aG dialout $USERNAME
RUN usermod -aG video $USERNAME

## initialize rosdep
RUN rosdep update

## set username
USER $USERNAME

## set variables
ENV ROS_DISTRO=$ROS_DISTRO
ENV ROS_DOMAIN_ID=42
ENV WORKSPACE=$WORKSPACE

## create workspace
RUN mkdir -p $WORKSPACE

# convenience scripts
RUN echo "source /opt/ros/$ROS_DISTRO/setup.bash" >> /home/$USERNAME/.bashrc
RUN echo "source $WORKSPACE/install/setup.bash && echo \"Sourced workspace\"" >> /home/$USERNAME/.bashrc

