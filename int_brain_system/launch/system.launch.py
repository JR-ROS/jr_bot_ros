#!/usr/bin/python3

from launch import LaunchDescription
from launch.actions import RegisterEventHandler, DeclareLaunchArgument
from launch.conditions import IfCondition
from launch.event_handlers import OnProcessExit
from launch.substitutions import LaunchConfiguration, Command, PathJoinSubstitution

from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    # Get package's share directory path
    int_brain_description_pkg_share = FindPackageShare('int_brain_description')
    int_brain_system_pkg_share = FindPackageShare('int_brain_system')

    rviz_arg = DeclareLaunchArgument("rviz", default_value="true", 
                                     description="Launch RViz2 with the robot model and controllers")

    # Launch Configurations to be used by nodes
    rviz = LaunchConfiguration("rviz")
    rviz_config_file = LaunchConfiguration("rviz_config_file", 
                            default=PathJoinSubstitution([
                                int_brain_system_pkg_share, 'config', 'view.rviz'
                            ])
                        )
    
    robot_controllers = PathJoinSubstitution(
        [
            int_brain_system_pkg_share,
            "config",
            "controllers.yaml",
        ]
    )

    # Path to the Xacro file
    xacro_path = PathJoinSubstitution([
        int_brain_description_pkg_share, 'urdf', 'int_brain.xacro'
    ])
    
    # Get URDF via xacro
    robot_description_content = Command([
        'xacro ', xacro_path,
        ' sim:=', 'false',
        ' controllers_yaml:=', robot_controllers,
    ])
    robot_description = {"robot_description": robot_description_content}

    control_node = Node(
        package="controller_manager",
        executable="ros2_control_node",
        parameters=[robot_description, robot_controllers],
        output="screen",
    )
    robot_state_pub_node = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        output="screen",
        parameters=[robot_description],
    )
    rviz_node = Node(
        package="rviz2",
        executable="rviz2",
        name="rviz2",
        output="screen",
        arguments=["-d", rviz_config_file],
        condition=IfCondition(rviz),
    )

    joint_state_broadcaster_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["joint_state_broadcaster", "--controller-manager", "/controller_manager"],
    )

    imu_broadcaster_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["imu_sensor_broadcaster", "--controller-manager", "/controller_manager"],
    )

    mecanum_drive_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["mecanum_drive_controller", "--controller-manager", "/controller_manager"],
    )

    # Delay rviz start after `joint_state_broadcaster`
    delay_rviz_after_joint_state_broadcaster_spawner = RegisterEventHandler(
        event_handler=OnProcessExit(
            target_action=joint_state_broadcaster_spawner,
            on_exit=[rviz_node],
        ) 
    )

    nodes = [
        control_node,
        robot_state_pub_node,
        joint_state_broadcaster_spawner,
        imu_broadcaster_spawner,
        mecanum_drive_controller_spawner,
        delay_rviz_after_joint_state_broadcaster_spawner,
    ]

    arguments = [
        rviz_arg
    ]

    return LaunchDescription(arguments+nodes)
