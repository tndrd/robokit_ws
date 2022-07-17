#!/usr/bin/env python

from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch import LaunchDescription
from utils.load_utils  import load_world
from ament_index_python.packages import get_package_share_directory
import os

def include_launch(launch_file, launch_arguments={}):
    
    package_share_directory = get_package_share_directory('robokit_webots')
    
    included_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            package_share_directory + f'/launch/{launch_file}'),
        launch_arguments=launch_arguments.items())
        

    return LaunchDescription([included_launch])

def simulation(world):
    return include_launch('sim_launch.py' , {'world': load_world(world)})

def robokit():
    return include_launch('robot_launch.py')

