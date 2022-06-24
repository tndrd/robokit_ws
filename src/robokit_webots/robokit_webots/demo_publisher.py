import rclpy
from math import sin
from rclpy.node import Node
from robokit_interfaces.msg import JointCommands

class DemoPublisher(Node):
  def __init__(self):
    super().__init__('demo_publisher')
    self.publisher = self.create_publisher(JointCommands, '/joints/commands', 10)
    
    timer_period = 0.1
    self.timer = self.create_timer(timer_period, self.timer_callback)
    self.x = 0

  def timer_callback(self):
    msg = JointCommands()
    msg.joint_names = ['pelvis_yaw', 'head_pitch']
    msg.values = [sin(self.x * 0.1), sin(self.x * 0.1)]
    print(f'Publishing {msg.joint_names} : {msg.values}')
    self.publisher.publish(msg)
    self.x+=1
 
def main(args=None):
  rclpy.init(args=args)
  demo_publisher = DemoPublisher()
  rclpy.spin(demo_publisher)
  demo_publisher.destroy_node()
  rclpy.shutdown()
 
if __name__ == '__main__':
  main()
