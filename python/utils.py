# Python
from collections import namedtuple
from enum import Enum
import math


class ObjectType(Enum):
    dummy = 0
    target = 1


class Object:
    def __init__(self, typ, x=0, y=0, diameter=0.01):
        self.typ = typ
        self.x = x
        self.y = y
        self.diameter = diameter


class BotParams:
    track_width = 0.13
    length = 0.1
    wheel_rad = 0.045 # Not true, but seems to make it work better


Point = namedtuple('Point', 'x y')
Vector = namedtuple('Vector', 'x y theta')
Controls = namedtuple('Controls', 'vel_req_left vel_req_right')


def abs_angle_diff(angle_1, angle_2):
    unwrapped_difference = abs(angle_1 - angle_2)
    return min(unwrapped_difference, 2 * math.pi - unwrapped_difference)


class G:
    ARENA_W = 1.5
    ARENA_H = 1.0
    OBSTACLE = ((0.7, 0.4), (0.7, 0.7), (1.05, 0.7), (1.05, 0.5))


class BotState:
    """ Struct to contain state params
    """

    def __init__(self):
        self.x_pos = 0.0
        self.y_pos = 0.0
        self.velocity = 0.0
        self.accel = 0.0
        self.yaw = 0.0


def wrap_angle(angle_in):
    if angle_in > math.pi:
        angle_in -= 2 * math.pi
    elif angle_in < -math.pi:
        angle_in += 2 * math.pi
    return angle_in


def transform_vector(pt, x, y, angle):
    return x + math.cos(angle) * pt[0] - math.sin(angle) * pt[1], y + math.sin(angle) * pt[0] + math.cos(angle) * pt[1]


def calc_distance(a_x, a_y, b_x, b_y):
    return math.sqrt((a_x - b_x) ** 2 + (a_y - b_y) ** 2)


def calc_angle(a_x, a_y, b_x, b_y):
    x_dif = b_x - a_x
    y_dif = b_y - a_y
    if x_dif is 0:
        if y_dif < 0:
            angle = math.pi / 2
        else:
            angle = -math.pi / 2
    if x_dif > 0:
        angle = math.atan(y_dif / x_dif)
    if x_dif < 0:
        angle = wrap_angle(math.pi + math.atan(y_dif / x_dif))
    return angle
