#!/usr/bin/env python

from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument
from utils.launch_utils import include_launch
from launch import LaunchDescription
import os

def generate_launch_description():
    
    robot_description   = LaunchConfiguration("robot_description")
    ros2_control_params = LaunchConfiguration("ros2_control_params")
    
    control_node = Node(
        package="controller_manager",
        executable="ros2_control_node",
        parameters=[{ 'robot_description': robot_description}, ros2_control_params],
        output={
            "stdout": "screen",
            "stderr": "screen",
        },
    )

    return LaunchDescription([
        DeclareLaunchArgument('robot_description'),
        DeclareLaunchArgument('ros2_control_params'),
        control_node
    ])

