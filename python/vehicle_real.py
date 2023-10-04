import math

import gpiozero

from motor import Motor
from utils import BotState, Controls, BotParams, wrap_angle, transform_vector, Vector

class VehicleReal:
    def __init__(self):
        # IO
        self.motorL = Motor(13, 3, 5, 6, -1.0)
        self.motorR = Motor(12, 2, 17, 27, 1.0)
        self.sonics = [gpiozero.DistanceSensor(echo=1,trigger=7, max_distance=4),
                       gpiozero.DistanceSensor(echo=23,trigger=24, max_distance=4)]
        self.sonic_pos = [Vector(0.055, -0.025, -math.pi / 8), 
                          Vector(0.055, 0.025, math.pi / 8)]

        # State Estimation
        self._state = BotState()
        self._state.x_pos = 0.15
        self._state.y_pos = 0.15
        self._state.yaw = 0.0

        self._vel_req_left = 0.0
        self._vel_req_right = 0.0

        self._vel_right = 0.0
        self._vel_left = 0.0
        self._yaw_rate = 0.0

    def set_control_inputs(self, controls):
        self._vel_req_left = controls.vel_req_left
        self.motorL.req_spd(self._vel_req_left)
        self._vel_req_right = controls.vel_req_right
        self.motorR.req_spd(self._vel_req_right)

    def get_state(self, delta_time):
        self._vel_right = self.motorR.spd
        self._vel_left = self.motorL.spd

        self._state.velocity = (self._vel_right + self._vel_left) * BotParams.wheel_rad / 2.0
        self._yaw_rate = (self._vel_right - self._vel_left) * BotParams.wheel_rad / BotParams.track_width

        self._state.x_pos += self._state.velocity * math.cos(self._state.yaw) * delta_time
        self._state.y_pos += self._state.velocity * math.sin(self._state.yaw) * delta_time

        self._state.yaw += self._yaw_rate * delta_time
        self._state.yaw = wrap_angle(self._state.yaw)
        # print(f"Vel:{self._state.velocity}, yaw:{self._yaw_rate}, (x,y):({self._state.x_pos}, {self._state.y_pos})")
        return self._state

    def get_sensors(self):
        distances = []
        p1 = []  # just for viz help, start point
        p2 = []  # just for viz help, end point
        for idx, sensor in enumerate(self.sonics):
            x1, y1 = transform_vector((self.sonic_pos[idx].x, self.sonic_pos[idx].y), self._state.x_pos, self._state.y_pos, self._state.yaw)
            p1.append((x1, y1))
            dist = sensor.distance
            distances.append(dist)
            g_theta = wrap_angle(self._state.yaw + self.sonic_pos[idx].theta)
            p2.append((x1 + dist*math.cos(g_theta), y1+dist*math.sin(g_theta)))

        return distances, p1, p2

