# Just Robotics Bot ROS Workspace

This is the workspace of our awesome project!


## Getting started
The documentation for managing the workspace is split into various READMEs, please read the correct file using the (relative) links below.

| File | Purpose |
| --- | --- |
| [Setup Scripts](/config/scripts/README.md) | Instructions and shell scripts to help quickly launch the workspace without dependence on Devcontainers. |
| [Containerization Source Code](https://github.com/JR-ROS/docker-containers) \[hosted externally\] | Source code for the Dockerfiles for to building the containers used by this project. |

### How to run Keyboard Teleop

1.  Make sure you have the workspace set up and running. You can follow the instructions in the [Setup Scripts](/config/scripts/README.md) to get started.

2.  Build the workspace if not done already.

    ```bash
    wsb
    ```

3.  Launch the core robot's ROS nodes.

    ```bash
    ros2 launch jr_bot_system system.launch.py
    ```

4.  In a new terminal, launch the keyboard teleop node.

    ```bash
    ros2 launch jr_bot_system teleop_keyboard.launch.py 
    ```

### How to change the device port.

1.  Find out the port your robot is connected to and make sure that you have the correct permissions to access it. See the [USB Guide](https://github.com/JR-ROS/ros_guides/blob/main/initial_setup/usb.md).

2.  Open [jr_bot.ros2_control.xacro](jr_bot_description/urdf/jr_bot.ros2_control.xacro) and edit the port:

    ```xml
    <xacro:macro name="jr_bot_ros2_control" params="device_addr:='/dev/ttyACM0'">
    ```

3.  Open [jr_bot.xacro](jr_bot_description/urdf/jr_bot.xacro) and edit the port:

    ```xml
    <xacro:jr_bot_ros2_control device_addr="/dev/ttyACM0" />
    ```

4.  Launch the robot's core nodes.

## Acknowledgements
The Devcontainer system of this project was inspired by the Wheelchair project at [RRC, IIIT Hyderabad](https://github.com/Smart-Wheelchair-RRC/). Please see their project [DockerForDevelopment](https://github.com/Smart-Wheelchair-RRC/DockerForDevelopment).
