#!/usr/bin/env python

"""Launch Webots Robokit-MV simulation."""

from launch_ros.actions import Node
from utils.launch_utils import include_launch
from launch import LaunchDescription

def generate_launch_description():
    
    ros2_control_launch = include_launch('ros2_control_launch.py')
    webots_hardware_launch = include_launch('webots_hardware_launch.py')
    webots_plain_launch = include_launch('webots_plain_launch.py')

    return LaunchDescription([
        ros2_control_launch,
        webots_hardware_launch,
        webots_plain_launch
    ])

