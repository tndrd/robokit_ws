#!/usr/bin/env python

from launch_ros.actions import Node
from utils.launch_utils import include_launch
from launch import LaunchDescription
import os

def generate_launch_description():
    
    DESCRIPTION_PATH = os.environ["ROBOKIT_DESCRIPTION"]

    robot_description = open(f"{DESCRIPTION_PATH}/urdf/robokit_mv_with_control_temp.urdf").read()
    ros2_control_params = f"{DESCRIPTION_PATH}/controller/ros2_control_config.yaml"
    controller_manager_timeout = ['--controller-manager-timeout', '100']
    controller_manager_prefix = 'python.exe' if os.name == 'nt' else ''

    use_deprecated_spawner_py = 'ROS_DISTRO' in os.environ and os.environ['ROS_DISTRO'] == 'foxy'

    control_node = Node(
        package="controller_manager",
        executable="ros2_control_node",
        parameters=[{ 'robot_description': robot_description}, ros2_control_params],
        output={
            "stdout": "screen",
            "stderr": "screen",
        },
    )

    position_controller_spawner = Node(
        package='controller_manager',
        executable='spawner' if not use_deprecated_spawner_py else 'spawner.py',
        #output='screen',
        prefix=controller_manager_prefix,
        arguments=['rk_joint_group_position_controller'] + controller_manager_timeout,
    )

    joint_state_broadcaster_spawner = Node(
        package='controller_manager',
        executable='spawner' if not use_deprecated_spawner_py else 'spawner.py',
        #output='screen',
        prefix=controller_manager_prefix,
        arguments=['rk_joint_state_broadcaster'] + controller_manager_timeout,
    )
    
    robot_state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        #output='screen',
        parameters=[{ 'robot_description': robot_description}],
    )

    joint_control_interface = Node(
        package='robokit_webots',
        executable='joint_control_interface',
        #output='screen',
    )

    control_demo = Node(
        package='robokit_webots',
        executable='demo_publisher',
        #output='screen'
    )

    return LaunchDescription([
        control_node,
        joint_state_broadcaster_spawner,
        position_controller_spawner,
        robot_state_publisher,
        joint_control_interface,
        control_demo
    ])

