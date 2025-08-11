FROM ghcr.io/eccentricOrange/int_brain_common:dev0.1

# Install Gz sim
RUN apt-get install --no-install-recommends -y \
    ros-$ROS_DISTRO-ros-gz-sim \
    ros-$ROS_DISTRO-ros-gz-interfaces \
    ros-$ROS_DISTRO-ros-gz-bridge

# Install plotjuggler
RUN apt-get install --no-install-recommends -y \
    ros-$ROS_DISTRO-rviz2 \
    ros-$ROS_DISTRO-rviz-imu-plugin \
    ros-$ROS_DISTRO-xacro \
    ros-$ROS_DISTRO-joint-state-publisher-gui \
    ros-$ROS_DISTRO-plotjuggler-ros