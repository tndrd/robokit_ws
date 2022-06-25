import rclpy
import threading
from rclpy.node import Node
from utils.load_utils import load_animation
from robokit_interfaces.msg import JointCommands

class Animation(Node):
    def __init__(self, name, SAMPLING_PERIOD=32):
        super().__init__('animation')
        
        source = load_animation(name)
        if source is None: return None

        self.publisher = self.create_publisher(JointCommands, '/joints/commands', 10)

        self.joints = source["header"]["joints"]
        self.keypoints = source["keypoints"]

        self.msg = JointCommands()
        self.msg.joint_names = self.joints

        self.rate = self.create_rate(1000/SAMPLING_PERIOD, self.get_clock())
    
    def play(self):
        self._send(self.keypoints[0]["values"])
        
        for keypoint, keypoint_next in zip(self.keypoints[:-1], self.keypoints[1:]):

            val1 = keypoint["values"]
            val2 = keypoint_next["values"]
            frames = keypoint_next["frames"]

            print(f"Set keypoints {val1} -> {val2} for {self.joints} in {frames} frames")
            self._transfer_keypoint(keypoint, keypoint_next)
            print("---\n")

        print("Animation done")


    def _transfer_keypoint(self, keypoint, keypoint_next):
        frames = keypoint_next["frames"]

        slopes = []
        biases = []

        if not rclpy.ok(): return

        for pose_start, pose_end in zip(keypoint["values"], keypoint_next["values"]):
            slopes.append((pose_end - pose_start) / frames)
            biases.append(pose_start)

        for frame in range(frames):
            values = [bias + slope*frame for slope, bias in zip(slopes, biases)]
            self._send(values)
            self.rate.sleep()

    def _send(self, values):
        print(f"Sending {values} to {self.joints}")
        self.msg.values = list(map(float,values))
        self.publisher.publish(self.msg) 


def main(args=None):
    rclpy.init(args=args)
    animation = Animation('demo.json')

    spin_thread = threading.Thread(target=rclpy.spin, args=(animation,), daemon=True)
    spin_thread.start()

    animation.play()
    
    animation.destroy_node()
    rclpy.shutdown()
    spin_thread.join()

if __name__ == '__main__':
    main()