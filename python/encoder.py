import gpiozero
import time
encoderL = gpiozero.RotaryEncoder(a=5, b=6,max_steps=100000) 
encoderR = gpiozero.RotaryEncoder(a=17, b=27, max_steps=10000)
step_per_rev = 32.0
gear_ratio = 114.7
wheel_rad = 5.46/2 # cm
prev_stepsR = 0
prev_time = time.perf_counter()
for i in range(10):
    time.sleep(1)

    now = time.perf_counter()
    dt = now-prev_time
    prev_time = now
    curr_stepsR = encoderR.steps
    spdR_steps = (curr_stepsR - prev_stepsR)/dt
    spdR = spdR_steps / step_per_rev / gear_ratio * wheel_rad #cm/s
    prev_stepsR = curr_stepsR
    
    print(f"CounterL: {-encoderL.steps}, CounterR: {encoderR.steps}, SpdR:{spdR:.2f}cm/s")
