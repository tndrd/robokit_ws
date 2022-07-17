import os
import rclpy
from rclpy.node import Node
from rclpy.parameter import Parameter
from ament_index_python.packages import get_package_share_directory
from utils.load_utils import load_description, load_ros2_controllers


class BringUpManager(Node):
    def __init__(self):
        super().__init__('bring_up')

        self.declare_parameter("environment")
        self.declare_parameter("controllers")

        self.directory = os.environ['ROBOKIT_DESCRIPTION'] + '/bring_up/'

        self.bring_up()
    
    def bring_up(self):
        print(f"Bringing up at {self.directory}...")
        os.system(f"mkdir -p {self.directory}")

        hw_name = None
        environment = self.get_parameter("environment").value
        controllers = self.get_parameter("controllers").value

        if environment == "webots":
            hw_name = "Webots simulation"
        elif environment == "reality":
            hw_name = "Reality"
        else:
            print(f"Error: Unknown environment {environment}")

        print(f"Robot is launching in {hw_name}")

        robot_description = load_description(environment, "/dev/pts/2", "38400", "1")
        ros2_controllers  = load_ros2_controllers(controllers)

        with open(self.directory + "robot_description.urdf", "w") as description_file:
            description_file.write(robot_description)
        print("Robot description configured")

        with open(self.directory + "controllers.yaml", "w") as controllers_file:
            os.system(f"cp {ros2_controllers} {self.directory}")
            os.system(f"mv {self.directory + controllers} {self.directory + 'controllers.yaml'}")
        print("Controllers configured")

        print("Bring up complete!")

    def clean(self):
        os.system(f"rm -rf {self.directory}")
        print("Bring up cleaned")

def main(args=None):
  rclpy.init(args=args)
  bring_up_manager = BringUpManager()
  rclpy.spin(bring_up_manager)
  bring_up_manager.clean()
  bring_up_manager.destroy_node()
  rclpy.shutdown()
 
if __name__ == '__main__':
  main()
