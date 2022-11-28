import Adafruit_BBIO.GPIO as GPIO
import time

# This does a few things:
# - Takes state information from the RF radio
# - Takes emergency braking information from both ultrasonic sensors
# - Combines both signals to determine final signal
# - Outputs final states to be outputted to Jetson


# input setup
state0 = 'P8_8'
state1 = 'P8_7'
stateu = 'P8_9' # state from ultrasonic sensor
GPIO.setup(state0, GPIO.IN)
GPIO.setup(state1, GPIO.IN)
GPIO.setup(stateu, GPIO.IN)

# output setup
GPIO.setup('P8_11', GPIO.OUT) # final output to jetson (might want to encode somehow later)

while True:
    time.sleep(1)
    # print("1: ", GPIO.input(state0))
    # print("0: ", GPIO.input(state1))
    rf_state = GPIO.input(state1) * 2 + GPIO.input(state0) 
    print(rf_state)
    ultrasonic_state = GPIO.input(stateu)
    print(ultrasonic_state)
    
    # final output stuff
    final_state = statechange(rf_state, ultrasonic_state)
    print(final_state)

def statechange(rf_state, ultrasonic_state):
    final_state = 0
    if ultrasonic_state == True:
        final_state = 4
    else:
        final_state = rf_state
    return final_state