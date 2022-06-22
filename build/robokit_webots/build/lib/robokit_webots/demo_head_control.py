import rclpy
from math import sin
from rclpy.node import Node
from std_msgs.msg import Float64MultiArray

class MinimalPublisher(Node):
  def __init__(self):

    super().__init__('demo_head_control')
    self.publisher_ = self.create_publisher(Float64MultiArray, '/rk_joint_group_position_controller/commands', 10)

    timer_period = 0.5
    self.timer = self.create_timer(timer_period, self.timer_callback)
    self.x = 0    

  def timer_callback(self):
    msg = Float64MultiArray()
    msg.data = [.0]*23
    msg.data[0] = sin(self.x)*0.5
    msg.data[1] = sin(self.x)*0.5
    print(msg.data)
    self.publisher_.publish(msg)
    self.x+=1
 
def main(args=None):

  rclpy.init(args=args)
  minimal_publisher = MinimalPublisher()
  rclpy.spin(minimal_publisher)

  minimal_publisher.destroy_node()
  rclpy.shutdown()
 
if __name__ == '__main__':
  main()
