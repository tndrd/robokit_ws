
#!/usr/bin/env python

"""Launch ros2_control"""

import os
from launch.substitutions import LaunchConfiguration
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument
from utils.load_utils import get_bringup_file
import launch

def generate_launch_description():
    
    robot_description = get_bringup_file("robot_description.urdf")

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

    joint_control_interface = Node(
        package='robokit_webots',
        executable='joint_control_interface',
        output='screen',
    )

    return LaunchDescription([
        joint_state_broadcaster_spawner,
        position_controller_spawner,
        robot_state_publisher,
        joint_control_interface
    ])

