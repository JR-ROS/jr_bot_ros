#!/usr/bin/python3

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription, AppendEnvironmentVariable
from launch.conditions import IfCondition, UnlessCondition
from launch.substitutions import LaunchConfiguration, PythonExpression, PathJoinSubstitution
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():
    # Get package's share directory path
    int_brain_description_pkg_share = FindPackageShare('int_brain_description')
    int_brain_gazebo_pkg_share = FindPackageShare('int_brain_gazebo')
    gz_sim_pkg_share = FindPackageShare("ros_gz_sim")

    # Retrieve launch configuration arguments
    use_sim_time = LaunchConfiguration('use_sim_time', default=True)
    world_file = LaunchConfiguration("world_file")
    rviz_config_file = LaunchConfiguration("rviz_config_file")
    gui = LaunchConfiguration("gui")
    
    gz_sim_with_gui = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            PathJoinSubstitution([gz_sim_pkg_share, "launch", "gz_sim.launch.py"])
        ),
        launch_arguments={
            "gz_args" : PythonExpression(["'", world_file, " -v 4 -r'"])
        }.items(),
        condition=IfCondition(gui)
    )

    gz_sim_without_gui = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            PathJoinSubstitution([gz_sim_pkg_share, "launch", "gz_sim.launch.py"])
        ),
        launch_arguments={
            "gz_args" : PythonExpression(["'", world_file, " -v 4 -r -s'"])
        }.items(),
        condition=UnlessCondition(gui)
    )

    spawn_int_brain_description_node = IncludeLaunchDescription(
       PythonLaunchDescriptionSource(
           PathJoinSubstitution([int_brain_gazebo_pkg_share, "launch", "model_gazebo_spawn.launch.py"])
       ),
    )

    rviz = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        output='screen',
        parameters=[{
            'use_sim_time': use_sim_time,
        }],
        arguments=['-d', rviz_config_file],
    )

    return LaunchDescription([
        # Declare launch arguments
        AppendEnvironmentVariable(
            name='GZ_SIM_RESOURCE_PATH',
            value=PathJoinSubstitution([int_brain_description_pkg_share, "worlds"])
        ),
 
        DeclareLaunchArgument("position_x", default_value="0.0"),
        DeclareLaunchArgument("position_y", default_value="0.0"),
        DeclareLaunchArgument("position_z", default_value="1.0"),
        DeclareLaunchArgument("orientation_roll", default_value="0.0"),
        DeclareLaunchArgument("orientation_pitch", default_value="0.0"),
        DeclareLaunchArgument("orientation_yaw", default_value="0.0"),

        DeclareLaunchArgument("gui", default_value="false", description="Start Gazebo GUI"),
        DeclareLaunchArgument(
            'world_file', 
            default_value=PathJoinSubstitution([int_brain_gazebo_pkg_share, 'worlds', 'empty.sdf']),
            description="Path to the world file."
        ),
        DeclareLaunchArgument('use_sim_time', default_value='true'),
        DeclareLaunchArgument(
            'rviz_config_file', 
            default_value=PathJoinSubstitution([int_brain_gazebo_pkg_share, 'config', 'gazebo.rviz']),
            description="Path to the rviz config file."
        ),
        DeclareLaunchArgument(
            "controllers_yaml_file",
            default_value='controllers.yaml',
            description="Path to the controllers YAML file"
        ),

        gz_sim_with_gui, gz_sim_without_gui,
        rviz, spawn_int_brain_description_node
    ])