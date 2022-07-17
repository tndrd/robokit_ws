#!/usr/bin/env python

from launch_ros.actions import Node
from utils.launch_utils import include_launch, simulation, robokit
from launch import LaunchDescription

def generate_launch_description():

    demo_publisher = Node(
        package='robokit_webots',
        executable='demo_publisher',
        output='screen'
    )

    return LaunchDescription([
        #simulation("robocup_training.wbt"),
        robokit(),
        demo_publisher
    ])

