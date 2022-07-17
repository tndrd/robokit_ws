#!/usr/bin/env python

from launch_ros.actions import Node, SetParameter
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from utils.launch_utils import include_launch
from launch import LaunchDescription
import os

def generate_launch_description():
    
    bring_up = Node(
        package='robokit_webots',
        executable='bring_up',
        output='screen',
        parameters=[
            {'environment': "reality"},
            {'controllers': "ros2_control_config.yaml"}
        ]
    )
    
    hardware_launch    = include_launch('robot_hardware_launch.py')
    controllers_launch = include_launch('controllers_launch.py')

    return LaunchDescription([
        bring_up,
        hardware_launch,
        controllers_launch
    ])

