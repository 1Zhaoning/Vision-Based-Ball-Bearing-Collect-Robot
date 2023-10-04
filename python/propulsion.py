import gpiozero
import time
import numpy as np

#Initialise PWM output pins
pwm_l = gpiozero.PWMOutputDevice(pin=12,active_high=True,initial_value=0,frequency=500)
pwm_r = gpiozero.PWMOutputDevice(pin=16,active_high=True,initial_value=0,frequency=500)

#Initialise shaft encoder pins
encoder_l = gpiozero.RotaryEncoder(a=5, b=6,max_steps=100000)
encoder_r = gpiozero.RotaryEncoder(a=14, b=15,max_steps=100000)

#Differential drive model of BotBot
class DiffDriveRobot:
    
    def __init__(self, dt=0.1, wheel_radius=0.03, wheel_sep=0.15):
        
        self.x = 0.0 # y-position
        self.y = 0.0 # y-position 
        self.th = 0.0 # orientation
        
        self.wl = 0.0 #rotational velocity left wheel
        self.wr = 0.0 #rotational velocity right wheel
        
        self.dt = dt #delta time for model computation
        
        self.r = wheel_radius #radius of wheel
        self.l = wheel_sep #separation of wheels
    

    #Measure steps taken between delta t and computer speed 
    def motor_measure(self):
        
        presteps_l = encoder_l.steps
        presteps_r = encoder_r.steps

        time.sleep(self.dt)

        wl = (encoder_l.steps-presteps_l)/self.dt
        wr = (encoder_r.steps-presteps_r)/self.dt
        
        return wl, wr
    
    # Veclocity motion model
    #Computes estimated velocity model of entire BotBot given differing speeds of wheels
    def base_velocity(self,wl,wr):
        
        v = (wl*self.r + wr*self.r)/2.0
        
        w = (wl - wr)/self.l
        
        return v, w
    
    # Kinematic motion model
    #Estimates changed position of BotBot and also write PWM value to motors
    def pose_update(self,duty_cycle_l,duty_cycle_r):
        
        pwm_l.value = duty_cycle_l
        pwm_r.value = duty_cycle_r

        self.wl, self.wr = self.motor_measure()
        
        v, w = self.base_velocity(self.wl,self.wr)
        
        self.x = self.x + self.dt*v*np.cos(self.th)
        self.y = self.y + self.dt*v*np.sin(self.th)
        self.th = self.th + w*self.dt
        
        return self.x, self.y, self.th


#Controller of movement and rotation of botbot
class MovementController:
    
    def __init__(self,Kp=0.1,Ki=0.01,wheel_radius=0.03, wheel_sep=0.15):
        
        self.Kp = Kp
        self.Ki = Ki
        self.r = wheel_radius
        self.l = wheel_sep
        self.e_sum_l = 0
        self.e_sum_r = 0
        
    #Proportional controller
    def p_control(self,w_desired,w_measured,e_sum):
        
        duty_cycle = min(max(-1,self.Kp*(w_desired-w_measured) + self.Ki*e_sum),1)
        
        e_sum = e_sum + (w_desired-w_measured)
        
        return duty_cycle, e_sum
        
    #Estimates required PWM to drive motors at 
    def drive(self,v_desired,w_desired,wl,wr):
        
        wl_desired = v_desired/self.r + self.l*w_desired/2 
        wr_desired = v_desired/self.r - self.l*w_desired/2
        
        duty_cycle_l,self.e_sum_l = self.p_control(wl_desired,wl,self.e_sum_l)
        duty_cycle_r,self.e_sum_r = self.p_control(wr_desired,wr,self.e_sum_r)

        return duty_cycle_l, duty_cycle_r