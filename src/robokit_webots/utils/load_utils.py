#!/usr/bin/env python

import os
import yaml
import json

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


