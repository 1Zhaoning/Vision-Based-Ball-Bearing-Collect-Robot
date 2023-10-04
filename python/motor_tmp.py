import gpiozero
import time

pwmL = gpiozero.PWMOutputDevice(pin=13,active_high=True,initial_value=0,frequency=5000)
pwmR = gpiozero.PWMOutputDevice(pin=12,active_high=True,initial_value=0,frequency=5000)
dirL = gpiozero.OutputDevice(pin=3)
dirR = gpiozero.OutputDevice(pin=2)
encoderL = gpiozero.RotaryEncoder(a=5, b=6,max_steps=100000)
encoderR = gpiozero.RotaryEncoder(a=17, b=27, max_steps=100000)


dirL.value = False
dirR.value = False
for j in range(2):
    pwmR.value = 0.1
    pwmL.value = 0.1
    #dirR.value = not dirR.value
    print(f"CounterL: {-encoderL.steps}, CounterR: {encoderR.steps}")
    time.sleep(1.0)

pwmR.off()
pwmL.off()
