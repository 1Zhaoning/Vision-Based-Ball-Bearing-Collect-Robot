import time

# BotBot
from arena_gui import ArenaGUI
from vehicle_model import VehicleModel
from path_planner import PathPlanner
from utils import Object, ObjectType, Controls
from scanner import Scanner
from multiprocessing import Process, Pipe
from target_detector_class import ObjectDetector


def main():
    gui = ArenaGUI()
    vehicle = VehicleModel()
    planner = PathPlanner()
    rate = 20.0

    envmap = [Object(ObjectType.dummy, 0.6, 0.7),
              Object(ObjectType.dummy, 1.4, 0.3, 0.02),
              Object(ObjectType.target, 1.0, 0.5)]
    gui.draw_map(envmap)

    scanner = Scanner()
    targets = scanner.scan_arena(envmap)
    planner.set_goals(targets)
    planner.find_best_goal(vehicle.get_state(0))


    video = 0
    parent_conn_detect, child_conn_detect = Pipe()
    detector = ObjectDetector()

    detection_detect = Process(target=detector.detect, args=(video, child_conn_detect))
    detection_detect.start()#Start multi-processing

    #
    while True:
        keypoints_info = parent_conn_detect.recv()
        print(keypoints_info[0])  # X coordinates of the keypoint
        print(keypoints_info[1])  # Y coordinates of the keypoint


        bot = vehicle.get_state(1.0 / rate)
        distances, p1, p2 = vehicle.get_sensors()
        next_controls = planner.get_controls(bot, envmap, p2)
        print(next_controls)
        vehicle.set_control_inputs(next_controls)

        gui.update_vehicle(bot)
        gui.update_sensors(p1, p2)
        gui.event_loop()
        time.sleep(1.0 / rate)
    detection_detect.join()


if __name__ == "__main__":
    main()