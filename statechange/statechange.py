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
out0 = 'P8_13'
out1 = 'P8_14'
GPIO.setup(out0, GPIO.OUT) # final output to jetson (might want to encode somehow later)
GPIO.setup(out1, GPIO.OUT) # final output to jetson (might want to encode somehow later)


def statechange(rf_state, ultrasonic_state):
    final_state = 0
    if ultrasonic_state == 1:
        final_state = 3
    else:
        final_state = rf_state
    return final_state


while True:
    time.sleep(1)
    # print("1: ", GPIO.input(state0))
    # print("0: ", GPIO.input(state1))
    rf_state = GPIO.input(state1) * 2 + GPIO.input(state0) 
    #print(rf_state)
    ultrasonic_state = GPIO.input(stateu)
    #print(ultrasonic_state)
    
    # final output stuff
    final_state = statechange(rf_state, ultrasonic_state)
    print("Final state: ")
    print(final_state)
    
    if (final_state == 0):
        GPIO.output(out1, GPIO.LOW)
        GPIO.output(out0, GPIO.LOW)  
    elif (final_state == 1):
        GPIO.output(out1, GPIO.LOW)
        GPIO.output(out0, GPIO.HIGH)  
    elif (final_state == 2):
        GPIO.output(out1, GPIO.HIGH)
        GPIO.output(out0, GPIO.LOW)  
    elif (final_state == 3):
        GPIO.output(out1, GPIO.HIGH)
        GPIO.output(out0, GPIO.HIGH)  
    else:
        GPIO.output(out1, GPIO.HIGH)
        GPIO.output(out0, GPIO.HIGH)
        print("Error!")
    print("Output LEDs: ")
    print(GPIO.input(out1))
    print(GPIO.input(out0))
