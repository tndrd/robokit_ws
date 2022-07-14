#!/usr/bin/env python

"""Launch Webots Hardware Interface"""

import os
from launch import LaunchDescription
from launch_ros.actions import Node
import launch
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    
    DESCRIPTION_PATH = os.environ["ROBOKIT_DESCRIPTION"]
    ros2_control_params = f"{DESCRIPTION_PATH}/controller/ros2_control_config.yaml"

    webots_hardware = open(f"{DESCRIPTION_PATH}/urdf/webots_hardware.urdf").read()

    robokit_driver = Node(
        package='webots_ros2_driver',
        executable='driver',
        output='screen',
        parameters=[
            {'robot_description': webots_hardware},
            {'use_sim_time': True},
            {'set_robot_state_publisher': True},
            ros2_control_params
        ]
    )

    return LaunchDescription([
        robokit_driver
    ])

