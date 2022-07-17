from setuptools import setup

package_name = 'robokit_webots'

setup(
    name=package_name,
    version='0.0.0',
    packages=['robokit_webots', 'utils'],
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        ('share/' + package_name + '/launch',
            ['launch/sim_launch.py',
             'launch/webots_hardware_launch.py',
             'launch/webots_plain_launch.py',
             'launch/controllers_launch.py',
             'launch/demo_launch.py',
             'launch/robot_launch.py',
             'launch/robot_hardware_launch.py'
            ]
        )
    ],
    install_requires=['setuptools', 'launch'],
    zip_safe=True,
    maintainer='tndrd',
    maintainer_email='tndrd@todo.todo',
    description='TODO: Package description',
    license='TODO: License declaration',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            f"demo_publisher = {package_name}.demo_publisher:main",
            f"joint_control_interface = {package_name}.joint_control_interface:main",
            f"animation = {package_name}.animation:main",
            f"bring_up = {package_name}.bring_up:main"
        ],
    },
)
