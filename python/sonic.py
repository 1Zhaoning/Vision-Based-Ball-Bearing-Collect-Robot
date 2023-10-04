import gpiozero
import time

sensor = gpiozero.DistanceSensor(echo=1,trigger=7, max_distance=4)
sensor2 = gpiozero.DistanceSensor(echo=23,trigger=24, max_distance=4)
while True:
    print(f"d1: {sensor.distance}, d2:{sensor2.distance}")
    time.sleep(1)
