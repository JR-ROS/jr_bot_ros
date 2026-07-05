#!/usr/bin/env python3

from os.path import join
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration, Command
from launch_ros.actions import Node


def generate_launch_description():
    # Get package's share directory path
    int_brain_description_path = get_package_share_directory('int_brain_description')

    rviz_config_file = LaunchConfiguration("rviz_config_file", default=join(int_brain_description_path, 'config', 'view.rviz'))

    # Path to the Xacro file
    xacro_path = join(int_brain_description_path, 'urdf', 'int_brain.xacro')

    # Launch the robot_state_publisher node
    robot_state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        output='screen',
        parameters=[{
                'robot_description': Command([
                    'xacro ', xacro_path,
                    ])
                }]
    )

    joint_state_publisher_node = Node(
        package="joint_state_publisher_gui",
        executable="joint_state_publisher_gui",
        output="screen",
    )

    rviz_node = Node(
        package="rviz2",
        executable="rviz2",
        name="rviz2",
        output="screen",
        arguments=['-d', rviz_config_file],
    )

    return LaunchDescription([
        DeclareLaunchArgument('rviz_config_file', default_value = rviz_config_file),

        robot_state_publisher,
        joint_state_publisher_node,
        rviz_node,
    ])

