#!/usr/bin/env python3

# When the car is in an autonomous state, run this code
# if it ever exits an autonomous state, exit this code
# if the car gets too close to something else, also exit this code

import Adafruit_BBIO.GPIO as GPIO
import time

trigger0 = 'P9_15'    # Pin to trigger the ultrasonic pulse
echo0    = 'P9_16'    # Pin to measure to pulse width related to the distance
trigger1 = 'P9_17'
echo1    = 'P9_18'

ms = 1000            # Trigger period in ms

# Triggers the breaking state
brake = False

GPIO.cleanup()
time.sleep(2)


def distance_measurement(TRIG,ECHO):
    GPIO.output(TRIG, True)
    time.sleep(0.00001)
    GPIO.output(TRIG, False)
    pulseStart = time.time()
    pulseEnd = time.time()
    counter = 0
    while GPIO.input(ECHO) == 0:
        pulseStart = time.time()
        counter += 1
    while GPIO.input(ECHO) == 1:
        pulseEnd = time.time()

    pulseDuration = pulseEnd - pulseStart
    distance = pulseDuration * 17150
    distance = round(distance, 2)
    return distance


# Configuration
print("trigger: [{}]".format(trigger0))
GPIO.setup(trigger0, GPIO.OUT) #Trigger
print("echo: [{}]".format(echo0))
GPIO.setup(echo0, GPIO.IN)  #Echo
GPIO.output(trigger0, False)

print("trigger: [{}]".format(trigger0))
GPIO.setup(trigger1, GPIO.OUT) #Trigger
print("echo: [{}]".format(echo1))
GPIO.setup(echo1, GPIO.IN)  #Echo
GPIO.output(trigger1, False)

print("Setup completed!")

distance0 = distance_measurement(trigger0, echo0)
distance1 = distance_measurement(trigger1, echo1)

while True:
    print("Distance of U0: [{}] cm.".format(distance0))
    time.sleep(0.5)
    print("Distance of U1: [{}] cm.".format(distance1))
    time.sleep(0.5)
    if (distance0 <= 5) or (distance1 <= 5):
        print("Emergency break state initiated.")
        brake = True
        print(brake)
        break
    else:
        distance0 = distance_measurement(trigger0, echo0)
        distance1 = distance_measurement(trigger1, echo1)

GPIO.cleanup()
print("Done")