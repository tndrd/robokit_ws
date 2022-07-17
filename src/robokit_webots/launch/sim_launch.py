#!/usr/bin/env python

"""Launch Webots Robokit-MV simulation."""

from launch_ros.actions import Node, SetParameter
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from utils.launch_utils import include_launch
from utils.load_utils   import load_world
from launch import LaunchDescription
import os

def generate_launch_description():

    world = LaunchConfiguration("world")
    
    bring_up = Node(
        package='robokit_webots',
        executable='bring_up',
        output='screen',
        parameters=[
            {'environment': "webots"},
            {'controllers': "ros2_control_config.yaml"}
        ]
    )
    
    webots_plain_launch    = include_launch('webots_plain_launch.py',    {'world': world})
    webots_hardware_launch = include_launch('webots_hardware_launch.py')
    controllers_launch     = include_launch('controllers_launch.py')

    return LaunchDescription([
        DeclareLaunchArgument('world'),
        bring_up,
        webots_plain_launch,
        webots_hardware_launch,
        controllers_launch
    ])

