#!/usr/bin/env python

import os
import yaml

def get_joint_names(filename='joint_names.yaml'):
    DESCRIPTION_PATH = os.environ['ROBOKIT_DESCRIPTION']
    joint_names = None
    with open(f'{DESCRIPTION_PATH}/config/{filename}') as yaml_file:
        yaml_content = yaml.load(yaml_file.read(), Loader=yaml.Loader)
        joint_names = yaml_content['controller_joint_names']
    return joint_names

print(get_joint_names())