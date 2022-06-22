#!/usr/bin/env python

from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch import LaunchDescription
from ament_index_python.packages import get_package_share_directory

def include_launch(launch_file, launch_arguments={}):
    
    package_share_directory = get_package_share_directory('robokit_webots')
    
    included_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            package_share_directory + f'/launch/{launch_file}'),
        launch_arguments=launch_arguments)
        

    return LaunchDescription([included_launch])

