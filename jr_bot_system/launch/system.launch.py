#!/usr/bin/python3

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, Command, PathJoinSubstitution, PythonExpression
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():
    jr_bot_description_pkg = FindPackageShare('jr_bot_description')
    jr_bot_system_pkg = FindPackageShare('jr_bot_system')

    rviz_arg = DeclareLaunchArgument("rviz", default_value="true")
    debug_arg = DeclareLaunchArgument("debug", default_value="false")
    stamp_twist_arg = DeclareLaunchArgument('stamp_twist', default_value='false')

    rviz = LaunchConfiguration("rviz")
    debug = LaunchConfiguration("debug")
    stamp_twist = LaunchConfiguration("stamp_twist")
    
    rviz_config_file = PathJoinSubstitution([jr_bot_system_pkg, 'config', 'view.rviz'])
    robot_controllers = PathJoinSubstitution([jr_bot_system_pkg, "config", "controllers.yaml"])
    ekf_params = PathJoinSubstitution([jr_bot_system_pkg, "config", "ekf.yaml"])
    xacro_path = PathJoinSubstitution([jr_bot_description_pkg, 'urdf', 'jr_bot.xacro'])
    
    robot_description_content = Command(['xacro ', xacro_path])
    robot_description = {"robot_description": robot_description_content}

    cmd_vel_topic = PythonExpression(["'/cmd_vel_stamped' if '", stamp_twist, "' == 'true' else '/cmd_vel'"])

    control_node = Node(
        package="controller_manager",
        executable="ros2_control_node",
        parameters=[robot_description, robot_controllers],
        output="screen",
        remappings=[
            ("/diff_drive_controller/cmd_vel", cmd_vel_topic),
            ("/diff_drive_controller/odom", "/jr_bot/odom"),
        ],
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

    diff_drive_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["diff_drive_controller", "--controller-manager", "/controller_manager"],
    )

    robot_localization = Node(
        package="robot_localization",
        executable="ekf_node",
        name="ekf_filter_node",
        output="screen",
        parameters=[ekf_params, robot_description],
    )

    twist_stamper = Node(
        package="twist_stamper",
        executable="twist_stamper",
        name="twist_stamper",
        output="screen",
        remappings=[('/cmd_vel_in', '/cmd_vel'), ('/cmd_vel_out', '/cmd_vel_stamped')],
        condition=IfCondition(stamp_twist)
    )

    madgwick_filter_node = Node(
        package='imu_filter_madgwick',
        executable='imu_filter_madgwick_node',
        name='imu_filter',
        output='screen',
        parameters=[{
            'use_mag': False,           # We only have a 6-DOF MPU6050 (No magnetometer)
            'publish_tf': False,        # Let robot_localization handle the TF tree
            'world_frame': 'enu',       # East-North-Up standard
        }],
        remappings=[
            # Input: The raw 6-DOF data from your ESP32
            ('/imu/data_raw', '/imu_sensor_broadcaster/imu'),
            # Output: The fused 9-DOF data containing the quaternion
            ('/imu/data', '/imu/data')
        ]
    )

    return LaunchDescription([
        rviz_arg,
        debug_arg,
        stamp_twist_arg,
        control_node,
        robot_state_pub_node,
        joint_state_broadcaster_spawner,
        imu_broadcaster_spawner,
        diff_drive_controller_spawner,
        twist_stamper,
        robot_localization,
        rviz_node,
        madgwick_filter_node
    ])