#!/usr/bin/env python

"""Launch plain Webots"""

import os
from launch import LaunchDescription
from launch_ros.actions import Node
import launch
from ament_index_python.packages import get_package_share_directory
from webots_ros2_driver.webots_launcher import WebotsLauncher

def generate_launch_description():
    
    DESCRIPTION_PATH = os.environ["ROBOKIT_DESCRIPTION"]
    
    webots = WebotsLauncher(
        world=f"{DESCRIPTION_PATH}/webots/worlds/robocup_training.wbt"
    )

    return LaunchDescription([
        webots,
        launch.actions.RegisterEventHandler(
            event_handler=launch.event_handlers.OnProcessExit(
                target_action=webots,
                on_exit=[launch.actions.EmitEvent(event=launch.events.Shutdown())],
            )
        ),
    ])

