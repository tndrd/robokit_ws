#!/usr/bin/env python

"""Launch Webots Hardware Interface"""

import os
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
import launch
from ament_index_python.packages import get_package_share_directory
from utils.load_utils import get_bringup_file, get_bringup_path

def generate_launch_description():
    
    robot_description   = get_bringup_file("robot_description.urdf")
    ros2_control_params = get_bringup_path("controllers.yaml")

    webots_driver = Node(
        package='webots_ros2_driver',
        executable='driver',
        output='screen',
        parameters=[
            {'robot_description': robot_description},
            {'use_sim_time': True},
            {'set_robot_state_publisher': True},
            ros2_control_params
        ]
    )

    return LaunchDescription([
        webots_driver
    ])

