#!/usr/bin/env python

# Copyright 1996-2021 Cyberbotics Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""Launch Webots Robokit-MV simulation."""

import os
from pathlib import Path
from launch.substitutions import LaunchConfiguration
from launch.actions import DeclareLaunchArgument
from launch import LaunchDescription
from launch_ros.actions import Node
import launch
from webots_ros2_driver.webots_launcher import WebotsLauncher

DESCRIPTION_PATH = "/home/tndrd/robokit_mv_description"

def generate_launch_description():
    
    ros2_control_params = f"{DESCRIPTION_PATH}/controller/ros2_control_config.yaml"

    robot_description = open(f"{DESCRIPTION_PATH}/urdf/robokit_mv.urdf").read()    

    controller_manager_timeout = ['--controller-manager-timeout', '100']
    controller_manager_prefix = 'python.exe' if os.name == 'nt' else ''

    use_deprecated_spawner_py = 'ROS_DISTRO' in os.environ and os.environ['ROS_DISTRO'] == 'foxy'

    position_controller_spawner = Node(
        package='controller_manager',
        executable='spawner' if not use_deprecated_spawner_py else 'spawner.py',
        output='screen',
        prefix=controller_manager_prefix,
        arguments=['rk_joint_group_position_controller'] + controller_manager_timeout,
    )

    joint_state_broadcaster_spawner = Node(
        package='controller_manager',
        executable='spawner' if not use_deprecated_spawner_py else 'spawner.py',
        output='screen',
        prefix=controller_manager_prefix,
        arguments=['rk_joint_state_broadcaster'] + controller_manager_timeout,
    )

    robot_state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        output='screen',
        parameters=[{
            'robot_description': robot_description
        }],
    )

    ## Webots specific segment

    webots = WebotsLauncher(
        world=f"{DESCRIPTION_PATH}/webots/worlds/robocup_training.wbt"
    )

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

    ## -----------------------

    return LaunchDescription([
        joint_state_broadcaster_spawner,
        position_controller_spawner,
        webots,
        robot_state_publisher,
        robokit_driver,
        launch.actions.RegisterEventHandler(
            event_handler=launch.event_handlers.OnProcessExit(
                target_action=webots,
                on_exit=[launch.actions.EmitEvent(event=launch.events.Shutdown())],
            )
        ),
    ])

