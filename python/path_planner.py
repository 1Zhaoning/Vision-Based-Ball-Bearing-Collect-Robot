# BotBot
import math
import copy
from typing import Sized
from utils import Controls, BotParams, wrap_angle, calc_distance, calc_angle, BotState, abs_angle_diff


class PathPlanner:
    def __init__(self):
        self.nav_stack = []                         # stack of indexes of goals sorted by distance
        self.goals = []                             # stack of found goals

        self.prox_collision = 0.1                   # distance to object where path unacceptable    
        self.prox_safe = 3 * self.prox_collision    # distance to object where considered safe
        self.prox_danger = 2 * self.prox_collision  # distance to object where navigating around deemed

        # weighting factors in path calculation
        self.alpha = 10            # weighting of distance to goal
        self.beta = 2             # weighting of how close bot is to facing goal
        self.danger = 5            # weighting of how much bot turns to avoids obstacles

        sense_timer = 5                        
        self.sense_timer = sense_timer              # time between sensor measurements stored in map 
        self.sense_time = 0                         # time before next measurement
        self.sense_map = CircularSensorBuffer()     # map of previous sensor values

        # creating tentacles 
        input_speeds = [0.75, 1.5]
        speeds = [0.0]
        for speed in input_speeds:
            speeds.append(speed)
            speeds.append(-1*speed)
        tentacles = []
        for j in speeds:
            for k in speeds:
                tentacles.append((j,k))
        self.tentacles = tentacles


    def is_sense_time(self):
        # function run every cycle path planning called to test if time to store another sensor value
        res = False
        self.sense_time -= 1
        if self.sense_time <= 0:
            res = True
            self.sense_time = self.sense_timer
        return res

    def set_goals(self, goals):
        # stores goals given in attributes
        for goal in goals:
            if goal not in self.goals:
                self.goals.append(goal)

    def find_best_goal(self, botState):
        # reorganizes the nav_stack in order of which goal is closest to the bot at current time
        self.nav_stack = []
        distances = []
        for i in range(len(self.goals)):
            distance = calc_distance(botState.x_pos, botState.y_pos, self.goals[i][0], self.goals[i][1])
            distances.append(distance)
            self.nav_stack.append(i)
            current = i
            while current is not 0 and distances[current - 1] > distances[current]:
                distances[current - 1], distances[current] = distances[current], distances[current - 1]
                self.nav_stack[current - 1], self.nav_stack[current] = self.nav_stack[current], self.nav_stack[current - 1]
                current -= 1

    def goal_capture(self):
        # function run when the goal currently being seeked is captured
        if not len(self.goals) is 0:
            captured_goal = self.nav_stack.pop(0)
            self.goals.pop(captured_goal)
        
        
    def play_out(self, bot, envmap, senseMap, steps, delta_time, vel_req_left , vel_req_right):
        # function to simulate a series of movements given input speeds
        # Assume instant reponse time of motors
        botState = copy.deepcopy(bot)

        # simulate movement making sure entire path causes no collisions
        for i in range(steps):
            botState.vel_right = vel_req_right
            botState.vel_left = vel_req_left

            botState.velocity = (botState.vel_right + botState.vel_left) * BotParams.wheel_rad / 2.0
            yaw_rate = (botState.vel_right - botState.vel_left) * BotParams.wheel_rad / BotParams.track_width

            botState.x_pos += botState.velocity * math.cos(botState.yaw) * delta_time
            botState.y_pos += botState.velocity * math.sin(botState.yaw) * delta_time

            botState.yaw += yaw_rate * delta_time
            botState.yaw = wrap_angle(botState.yaw)
            if(self.check_collision(botState, envmap, senseMap)):
                print("Possible Collision")
                return Score(0, 0, math.inf) # Segfault??

        # define goal position from next goal on stack
        goal_x_pos = self.goals[self.nav_stack[0]][0]
        goal_y_pos = self.goals[self.nav_stack[0]][1]

        # calculate difference in current yaw and yaw facing target
        yaw_score = self.calc_yaw_score(botState, goal_x_pos, goal_y_pos)

        # calculate distance score
        dis = self.alpha*calc_distance(goal_x_pos, goal_y_pos, botState.x_pos, botState.y_pos)**2

        # calculate yaw score
        yaw = self.beta * yaw_score**2

        # old safe value, being kept in if needed to reimplement
        # safe = self.safe * self.check_safe(botState, envmap, senseMap)**2

        # calculate danger score
        danger = self.danger * self.check_danger(botState, envmap, senseMap)

        score = Score(dis, yaw, danger)

        return score

    def check_collision(self, bot, envmap, senseMap):
        # checks if bot is in unacceptable proximity to obstacle
        # returns boolean, true if collision occured
        for sense in senseMap:
            angle_obj = math.atan2(sense[1]-bot.y_pos, sense[0]-bot.x_pos)
            if calc_distance(bot.x_pos, bot.y_pos, sense[0], sense[1]) < self.prox_collision and abs_angle_diff(angle_obj, bot.yaw) < math.pi/3:
                return True
        return False
    
    def check_safe(self,bot, envmap, senseMap):
        # old deprecated function
        # intended to promote bot going to free space it knew was safe
        # caused lots of bugs with deciding which free space to go into
        # often caused the bot to stay still as approaching some free space would cause it to go away from other free space
        res = math.inf
        dis_bot = []
        dis_goal = []
        goal_x_pos = self.goals[self.nav_stack[0]][0]
        goal_y_pos = self.goals[self.nav_stack[0]][1]
        for sense in senseMap:
            dis = calc_distance(bot.x_pos, bot.y_pos, sense[0], sense[1])
            if dis > self.prox_safe:
                dis_bot.append(dis)
                dis_goal.append(calc_distance(sense[0],sense[1], goal_x_pos, goal_y_pos))
        if len(dis_bot) is not 0:
            res = dis_bot[dis_goal.index(min(dis_goal))]
            #res = sum(dis_bot)/len(dis_bot)
        return res

    def check_danger(self, bot, envmap, senseMap):
        # checks the danger weighting of the current bot position
        # danger weight output is maxed at math.pi at normal values
        # if bot gets close to obstacles can reduce this value by facing away from them
        # to stop indecision an average obstacle point is calculated

        res = math.pi   # max value
        dangerMap = []  # array containing all points that are within danger proximity

        # find all points in senseMap that are in danger proximity
        for sense in senseMap:
            dis = calc_distance(bot.x_pos, bot.y_pos, sense[0], sense[1])
            if dis < self.prox_danger:
                dangerMap.append(sense)

        # if there are points in danger proximity
        if len(dangerMap) > 0:
            x_pos = 0
            y_pos = 0

            # calculate the average danger point
            for point in dangerMap:
                x_pos += point[0]
                y_pos += point[1]
            x_pos = x_pos / len(dangerMap)
            y_pos = y_pos / len(dangerMap)
            
            # calculate angle between robot and this average danger point
            yaw_score = self.calc_yaw_score(bot, x_pos, y_pos)

            # return higher weighting based on how little angle is
            res = math.pi - yaw_score
        
        return res

    def calc_yaw_score(self, bot, x_pos, y_pos):
        # calculate positive angle between bot yaw angle and angle from bot to a given point
        goal_yaw = calc_angle(bot.x_pos, bot.y_pos, x_pos, y_pos)
        yaw_score = abs(goal_yaw - bot.yaw)
        if yaw_score > math.pi:
            yaw_score = 2*math.pi - yaw_score
        return yaw_score
        

    def plan(self, bot, envmap, senseMap):
        costs = [] # weight of current bot location
        scores = [] # weight broken down by reason
        steps = 5 # number of steps the bot plans forward
        delta_time = 0.1 # time between each step
        
        # for each velocity in tentacles
        for vel_req_left, vel_req_right in self.tentacles:
            # calculate score from each position
            score = self.play_out(bot, envmap, senseMap, steps, delta_time, vel_req_left , vel_req_right)
            scores.append(score)
            costs.append(score.get_score())
        
        # find the minimum score
        min_index = costs.index(min(costs))
        # debugging printing score values
        # scores[min_index].print_score()        
        # return the movement associated with the minimum score
        return self.tentacles[min_index]

    def get_controls(self, bot, envmap, senseMap):

        # if no goal found
        if len(self.goals) is 0:
            return Controls(0,0)

        # if time to update sensor map
        if self.is_sense_time():
            for sense in senseMap:
                self.sense_map.add(sense)                
        
        # get currently held sense map
        sense_map = self.sense_map.get_arr()
        # calculate best path
        path = self.plan(bot, envmap, sense_map)

        # return best path
        return Controls(path[0], path[1])

class Score:
    # class to breakdown score values
    def __init__(self, dis, yaw, danger):
        self.dis = dis
        self.yaw = yaw
        self.danger = danger

    def print_score(self):
        # can be called to see how score was calculated =
        # used for pulling attributes to change weightings
        string = "dis: " + str(round(self.dis,4)) +  ", " + \
                "yaw: " + str(round(self.yaw,4)) +  ", " + \
                "danger: " + str(round(self.danger,4))
        print(string)
    
    def get_score(self):
        return self.dis + self.yaw + self.danger

class CircularSensorBuffer:
    # an array that holds the last capacity values and can return them in an array

    capacity = 40   # total number of elements that can be held
    size = 0    # current amount of elements held

    def __init__(self):
        self.tail = 0
        self.array = [None] * self.capacity
    
    def is_full(self):
        # checks if buffer is full
        return self.size == self.capacity

    def add(self, element):  
        # adds an element to an empty spot or replaces the earliest put in value currently in list

        # if list not full increase size
        if not self.is_full():
            self.size += 1    
      
        self.array[self.tail] = element
        self.tail = (self.tail + 1) % self.capacity
    
    def get_arr(self):
        # return slice of array so only return values that were input
        return self.array[0:self.size]

        

