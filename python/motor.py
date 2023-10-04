import gpiozero
import time
import math

class Motor:
    step_per_rev = 32.0
    gear_ratio = 114.7

    def __init__(self, pwn_pin, dir_pin, encA_pin, ancB_pin, step_factor = 1.0, start_dir = False):
        # GPIO
        self.pwm = gpiozero.PWMOutputDevice(pin=pwn_pin,active_high=True,initial_value=0,frequency=5000)
        self.dir = gpiozero.OutputDevice(pin=dir_pin)
        self.dir.value = start_dir
        self.encoder = gpiozero.RotaryEncoder(a=encA_pin, b=ancB_pin,max_steps=100000)
        self.step_factor = step_factor # Reverse direction readings to pos is forward
        # State Vars
        self.prev_time = time.perf_counter()
        self.prev_steps = 0
        self.spd = 0

        #Controller
        self.Kp = 0.01
        self.Ki = 0.02
        self.error_sum = 0.0
        self.smoothing = 0.0 # Smooth speed reading to reduce jerk

    def read_encoder(self):
        now = time.perf_counter()
        dt = now - self.prev_time
        self.prev_time = now
        curr_steps = self.encoder.steps * self.step_factor
        spd_steps = (curr_steps - self.prev_steps)/dt
        new_spd = spd_steps / Motor.step_per_rev / Motor.gear_ratio * 2 * math.pi
        self.spd = self.smoothing*self.spd + (1.0-self.smoothing) * new_spd
        self.prev_steps = curr_steps

    def req_spd(self, trg_spd):
        # Target in rads / sec

        self.read_encoder()
        error = trg_spd - self.spd
        self.error_sum += error * self.Ki

        # Anti-Windup method: clamp error so output from Ki alone never greater than 1
        self.error_sum = min(0.5, max(self.error_sum, 0.0))

        out = (self.Kp * error) + self.error_sum
        out = min(0.5, max(out, 0.0)) # Clamp pwm to valid range
        self.pwm.value = out
        # print(f"TrgSpd:{trg_spd:.1f}, Spd:{self.spd:.2f}, PWM:{out:.2f}, Steps:{self.encoder.steps}")

    

