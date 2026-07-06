#!/usr/bin/python3

from launch import LaunchDescription
from launch.substitutions import PathJoinSubstitution, LaunchConfiguration

from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    # Corrected package name
    jr_bot_system_pkg_share = FindPackageShare('jr_bot_system')

    teleop_params = PathJoinSubstitution([jr_bot_system_pkg_share, 'config', 'teleop_params.yaml'])
    rviz_config_file = LaunchConfiguration("rviz_config_file", 
                            default=PathJoinSubstitution([
                                jr_bot_system_pkg_share, 'config', 'view.rviz'
                            ])
                        )

    rviz_node = Node(
        package="rviz2",
        executable="rviz2",
        name="rviz2",
        output="screen",
        arguments=["-d", rviz_config_file],
    )

    teleop_node = Node(
        package='teleop_twist_joy',
        executable='teleop_node',
        name='teleop_twist_joy_node',
        parameters=[teleop_params],
        remappings=[
            ('/cmd_vel', '/cmd_vel_raw')
        ]
    )

    game_controller_node = Node(
        package='joy',
        executable='game_controller_node',
        name='game_controller_node',
    )

    # Corrected package name here as well
    imu_feedback_node = Node(
        package='jr_bot_system',
        executable='imu_feedback_node',
        name='imu_feedback_node',
        parameters=[teleop_params],
    )

    lifecycle_manager = Node(
        package='nav2_lifecycle_manager',
        executable='lifecycle_manager',
        name='lifecycle_manager',
        output='screen',
        parameters=[{
            'use_sim_time': False,
            'autostart': True,
            'node_names': ['velocity_smoother']
        }]
    )

    velocity_smoother_node = Node(
        package='nav2_velocity_smoother',
        executable='velocity_smoother',
        name='velocity_smoother',
        output='screen',
        parameters=[teleop_params],
        remappings=[
            ('/cmd_vel', '/cmd_vel_raw'),
            ('/cmd_vel_smoothed', '/cmd_vel')
        ]
    )

    nodes = [
        teleop_node, game_controller_node,
        imu_feedback_node, rviz_node,
        velocity_smoother_node, lifecycle_manager
    ]

    return LaunchDescription(nodes)