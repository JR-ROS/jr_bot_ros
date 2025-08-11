FROM ghcr.io/eccentricOrange/int_brain_sbc:dev0.1

WORKDIR $WORKSPACE

# Copy your source code into the container
COPY ./src ./src

# Install ROS dependencies for your specific packages
RUN source /opt/ros/humble/setup.bash && \
rosdep install -i --from-path src -y --rosdistro humble

# Build the ROS 2 workspace
RUN source /opt/ros/humble/setup.bash && \
colcon build --symlink-install

# Set up the entrypoint to source the workspace and open a shell
CMD ["bash", "-c", "source /ros2_ws/install/setup.bash && bash"]