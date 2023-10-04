## Python
import math

## MMS
from utils import BotState, BotParams, Controls, wrap_angle, Vector, Point, transform_vector, G, abs_angle_diff


class VehicleModel:
    def __init__(self):
        self._state = BotState()
        self._state.x_pos = 0.3
        self._state.y_pos = 0.7
        self._state.yaw = math.pi

        self._vel_req_left = 0.0
        self._vel_req_right = 0.0

        self._vel_right = 0.0
        self._vel_left = 0.0
        self._yaw_rate = 0.0
        self._ultrasonics = (Vector(0.07, 0.04, math.pi / 24), Vector(0.07, -0.04, -math.pi / 24), Vector(0.06, -0.06, -math.pi / 6), Vector(0.06, 0.06, math.pi / 6)  )

    def set_control_inputs(self, controls):
        self._vel_req_left = controls.vel_req_left
        self._vel_req_right = controls.vel_req_right

    def get_state(self, delta_time):
        # Assume instant reponse time of motors
        self._vel_right = self._vel_req_right
        self._vel_left = self._vel_req_left

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

        for sensor in self._ultrasonics:
            x1, y1 = transform_vector((sensor.x, sensor.y), self._state.x_pos, self._state.y_pos, self._state.yaw)
            p1.append((x1, y1))

            g_theta = wrap_angle(self._state.yaw + sensor.theta)
            # Check wall collision first
            # TODO: Deal with edge case when exactly perpendicular to wall
            # Check top and bottom intersections
            if g_theta > 0:
                d_h = G.ARENA_H - y1
                d = d_h/math.sin(g_theta)
            else:
                d = y1/math.sin(-g_theta)

            # Check left right intersections
            if wrap_angle(g_theta - math.pi/2) > 0:
                d = min(d, x1/abs(math.cos(g_theta)))
            else:
                d_w = G.ARENA_W - x1
                d = min(d, d_w/math.cos(g_theta))

            # Check For collisions with line segments of the obstacle
            # Just one line segment for now
            seg_p1 = G.OBSTACLE[0]
            seg_p2 = G.OBSTACLE[1]
            seg_1dx = seg_p1[0] - x1
            seg_1dy = seg_p1[1] - y1
            seg_1ang = math.atan2(seg_1dy, seg_1dx)
            seg_2dx = seg_p2[0] - x1
            seg_2dy = seg_p2[1] - y1
            seg_2ang = math.atan2(seg_2dy, seg_2dx)
            seg_span = abs_angle_diff(seg_1ang, seg_2ang)
            if abs_angle_diff(g_theta, seg_1ang) < seg_span and abs_angle_diff(g_theta, seg_2ang) < seg_span:
                # There is collision
                d_new = (seg_p2[0] - x1)/math.cos(g_theta)
                d = min(d, d_new)

            p2.append((x1 + d*math.cos(g_theta), y1+d*math.sin(g_theta)))
            distances.append(d)

        return distances, p1, p2
