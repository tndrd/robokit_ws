#!/usr/bin/env python

from launch_ros.actions import Node
from utils.launch_utils import include_launch
from launch import LaunchDescription

def generate_launch_description():
    
    sim_launch = include_launch('sim_launch.py')

    control_demo = Node(
        package='robokit_webots',
        executable='demo_head_control',
        output='screen'
    )

    return LaunchDescription([
        sim_launch,
        control_demo
    ])

