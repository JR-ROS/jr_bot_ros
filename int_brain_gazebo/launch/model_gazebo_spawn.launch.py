#!/usr/bin/python3

from launch import LaunchDescription
from launch.substitutions import LaunchConfiguration, Command, PathJoinSubstitution
from launch_ros.actions import Node
from launch.event_handlers import OnProcessExit
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():
    # Get package's share directory path
    int_brain_description_pkg_share = FindPackageShare('int_brain_description')
    int_brain_gazebo_pkg_share = FindPackageShare('int_brain_gazebo')

    # Launch Configurations to be used by nodes
    position_x = LaunchConfiguration("position_x")
    position_y = LaunchConfiguration("position_y")
    position_z = LaunchConfiguration("position_z")
    orientation_roll = LaunchConfiguration("orientation_roll")
    orientation_pitch = LaunchConfiguration("orientation_pitch")
    orientation_yaw = LaunchConfiguration("orientation_yaw")

    controllers_yaml_file = LaunchConfiguration("controllers_yaml_file", default="controllers.yaml")

    # Path to the Xacro file
    xacro_path = PathJoinSubstitution([
        int_brain_description_pkg_share, 'urdf', 'int_brain.xacro'
    ])

    # Path to the controllers YAML file
    controllers_yaml = PathJoinSubstitution([
        int_brain_gazebo_pkg_share, 'config', controllers_yaml_file
    ])

    # Robot description command
    robot_description_command = Command([
        'xacro ', xacro_path,
        ' sim:=', 'true',
        ' controllers_yaml:=', controllers_yaml
    ])

    # Robot State Publisher Node
    robot_state_publisher_node = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        output='screen',
        parameters=[{
            'robot_description': robot_description_command,
            'use_sim_time': True
        }]
    )

    # Gazebo Spawn Entity Node
    gz_spawn_entity_node = Node(
        package="ros_gz_sim",
        executable="create",
        arguments=[
            '-topic', "/robot_description",
            '-name', 'int_brain',
            '-allow_renaming', 'true',
            '-x', position_x,
            '-y', position_y,
            '-z', position_z,
            '-R', orientation_roll,
            '-P', orientation_pitch,
            '-Y', orientation_yaw
        ],
        output="screen"
    )

    ros_gz_bridge_node = Node(
        package="ros_gz_bridge",
        executable="parameter_bridge",
        arguments=[
            "/clock@rosgraph_msgs/msg/Clock[gz.msgs.Clock",
            "/odom@nav_msgs/msg/Odometry[gz.msgs.Odometry",
            "/tf@tf2_msgs/msg/TFMessage[gz.msgs.Pose_V",
            "/cmd_vel@geometry_msgs/msg/Twist@gz.msgs.Twist",
        ],
        parameters=[{'use_sim_time': True}]
    )

    joint_state_broadcaster_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["joint_state_broadcaster", "--controller-manager", "/controller_manager"],
        output="screen",
    )

    return LaunchDescription([
        # Robot description and state
        robot_state_publisher_node,

        # Spawning and Bridges
        gz_spawn_entity_node,
        ros_gz_bridge_node,

        # ROS2 Control
        joint_state_broadcaster_spawner
    ])