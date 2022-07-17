#!/usr/bin/env python

from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument
from utils.launch_utils import include_launch
from launch import LaunchDescription
from utils.load_utils import get_bringup_file, get_bringup_path
import os

def generate_launch_description():
    
    robot_description   = get_bringup_file("robot_description.urdf")
    ros2_control_params = get_bringup_path("controllers.yaml")
    
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
        control_node
    ])

