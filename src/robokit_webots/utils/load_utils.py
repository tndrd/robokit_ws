#!/usr/bin/env python

import os
import yaml
import json
from launch.substitutions import Command, FindExecutable
import xacro
from launch_ros.parameter_descriptions import ParameterValue

DESCR_NAME = "robot_description.urdf"

def load_joint_names(filename='joint_names.yaml'):
    DESCRIPTION_PATH = os.environ['ROBOKIT_DESCRIPTION']
    joint_names = None
    path = f'{DESCRIPTION_PATH}/config/{filename}'
    try:
        with open(path) as yaml_file:
            yaml_content = yaml.load(yaml_file.read(), Loader=yaml.Loader)
            joint_names = yaml_content['controller_joint_names']
    except FileNotFoundError:
        print(f"Error: joint names file not found at {path})")
    return joint_names

def load_animation(filename):
    DESCRIPTION_PATH = os.environ['ROBOKIT_DESCRIPTION']
    animation = None
    path = f'{DESCRIPTION_PATH}/animation/{filename}'
    try:
        with open(path) as json_file:
            animation = json.loads(json_file.read())
    except FileNotFoundError:
        print(f"Error: animation file not found at {path}")
    return animation

def load_ros2_controllers(filename):
    DESCRIPTION_PATH = os.environ['ROBOKIT_DESCRIPTION']
    path = f'{DESCRIPTION_PATH}/controller/{filename}'
    try:
        with open(path) as dummy: pass
    except FileNotFoundError:
        print(f"Error: ros2_controllers config file not found at {path}")
    return path


def load_world(filename):
    DESCRIPTION_PATH = os.environ['ROBOKIT_DESCRIPTION']
    path = f'{DESCRIPTION_PATH}/webots/worlds/{filename}'
    #try:
    ##    with open(path) as dummy: pass
    #except FileNotFoundError:
    #    print(f"Error: world file not found at {path}")
    return path

def load_description(environment, port="none", baud="none", timeout="none"):
    DESCRIPTION_PATH    = os.environ['ROBOKIT_DESCRIPTION']
    robokit_description = f"{DESCRIPTION_PATH}/urdf/robokit_mv.urdf.xacro"

    use_sim = "false"
    if environment   == "webots": use_sim = "true"
    elif environment == "reality": pass
    else:
        print(f'Error: unknown environment {environment}')
        raise RuntimeError

    doc = xacro.process_file(robokit_description, 
        mappings={  'use_sim':  use_sim,
                    'port':     str(port),
                    'baudrate': str(baud),
                    'timeout':  str(timeout)  })
    
    description = doc.toxml()

    return description

def get_bringup_path(file):
    DESCRIPTION_PATH = os.environ['ROBOKIT_DESCRIPTION']
    path = f"{DESCRIPTION_PATH}/bring_up/{file}"
    try:
        with open(path) as bringed_up_file: pass
    except FileNotFoundError:
        print(f"Error: bringed up file not found at {path}")
    return path

def get_bringup_file(file):
    DESCRIPTION_PATH = os.environ['ROBOKIT_DESCRIPTION']
    path = f"{DESCRIPTION_PATH}/bring_up/{file}"
    content = None
    try:
        with open(path) as bringed_up_file:
            content = bringed_up_file.read()
    except FileNotFoundError:
        print(f"Error: bringed up file not found at {path}")
    return content