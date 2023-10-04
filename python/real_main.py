import time
import math

from vehicle_real import VehicleReal
from arena_gui import ArenaGUI
from utils import Object, ObjectType, Controls
from scanner import Scanner  # Need to replace this with real variant
from path_planner import PathPlanner
from multiprocessing import Process, Pipe
from target_detector_parallel import ObjectDetector


def main():
    gui = ArenaGUI()
    vehicle = VehicleReal()
    planner = PathPlanner()
    detector = ObjectDetector()
    for i in range(3):
        print(f"Waiting...{10 - i} sec")
        time.sleep(1)

    rate = 15.0

    envmap = [Object(ObjectType.target, 0.45, 0.45)]  # Remember init pos is (0.15, 0.15)
    gui.draw_map(envmap)
    bot = vehicle.get_state(0.0)

    scanner = Scanner()
    targets = scanner.scan_arena(envmap)
    planner.set_goals(targets)
    planner.find_best_goal(vehicle.get_state(0))

    video = 0  # set video=0 to use the camera

    # for i in range(int(rate*2.0)):
    while True:
        # Multi-threading the detection software
        parent_conn_detect, child_conn_detect = Pipe()
        detection_detect = Process(target=detector.detect(video, child_conn_detect))
        detection_detect.start()
        keypoints_image_coord = parent_conn_detect.recv()



###
        bot = vehicle.get_state(1.0 / rate)
        distances, p1, p2 = vehicle.get_sensors()

        # vehicle.set_control_inputs(Controls(0.0, 1.0))
        next_controls = planner.get_controls(bot, envmap, p2)
        vehicle.set_control_inputs(next_controls)

        # print(f"x:{bot.x_pos:.2f}, y:{bot.y_pos:.2f}, yaw:{bot.yaw*180/math.pi:.0f}")
        gui.update_vehicle(bot)
        gui.update_sensors(p1, p2)
        gui.event_loop()
        time.sleep(1.0 / rate)


if __name__ == "__main__":
    main()
