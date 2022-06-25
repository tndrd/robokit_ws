import rclpy
#!/usr/bin/env python

from rclpy.node import Node
from robokit_interfaces.msg import JointCommands
from std_msgs.msg import Float64MultiArray
from utils.load_utils import load_joint_names

class JointControlInterface(Node):
    def __init__(self, joint_names_path='joint_names.yaml', interface_topic='/joints/commands', ros2control_topic='/rk_joint_group_position_controller/commands'):
        super().__init__('joint_control_interface')
        self.joint_names = load_joint_names(joint_names_path)
        self.interface_sub   = self.create_subscription(JointCommands, interface_topic, self.joint_commands_callback, 10)
        self.ros2control_pub = self.create_publisher(Float64MultiArray, ros2control_topic, 10)

        self.idx_map = {}

        for joint, idx in zip(self.joint_names, range(len(self.joint_names))):
            self.idx_map[joint] = idx

    def joint_commands_callback(self, msg):
        ros2_msg = Float64MultiArray()
        ros2_msg.data = [.0] * len(self.joint_names)

        for joint, value in zip(msg.joint_names, msg.values):
            if joint not in self.idx_map.keys():
                print(f"Error: joint {joint} is not defined")
                continue 
            joint_idx = self.idx_map[joint]
            ros2_msg.data[joint_idx] = value
        
        self.ros2control_pub.publish(ros2_msg)


def main(args=None):
    rclpy.init(args=args)
    joint_control_interface = JointControlInterface()
    rclpy.spin(joint_control_interface)
    joint_control_interface.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
