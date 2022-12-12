"""
--------------------------------------------------------------------------
State Driver
--------------------------------------------------------------------------
License:   
Copyright 2022 Eunice Tan
Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this 
list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation 
and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors 
may be used to endorse or promote products derived from this software without 
specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
--------------------------------------------------------------------------
# This does a few things:
# - Takes state information from the RF radio
# - Takes emergency braking information from both ultrasonic sensors
# - Combines both signals to determine final signal
# - Outputs final states to be outputted to Jetson

Software for State Driver (and the rest of the project):
Full description at https://www.hackster.io/eunice-tan/remote-controlled-lighting-system-b601c9
"""


import Adafruit_BBIO.GPIO as GPIO
import time

# ------------------------------------------------------------------------
# Global variables
# ------------------------------------------------------------------------
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


# ------------------------------------------------------------------------
# State Change: change this logic to modify final states 
# ------------------------------------------------------------------------
def statechange(rf_state, ultrasonic_state):
    final_state = 0
    if ultrasonic_state == 1:
        final_state = 3
    else:
        final_state = rf_state
    return final_state


# ------------------------------------------------------------------------
# TODO: TFT LCD Class (put here)
# ------------------------------------------------------------------------

# ------------------------------------------------------------------------
# TODO: LED Strip Lights Class (put here)
# ------------------------------------------------------------------------

# ------------------------------------------------------------------------
# Main Loop
# ------------------------------------------------------------------------

while True:
    time.sleep(1)
    #print("1: ", GPIO.input(state0))
    # rint("0: ", GPIO.input(state1))
    rf_state = GPIO.input(state1) * 2 + GPIO.input(state0) 
    #print(rf_state)
    ultrasonic_state = GPIO.input(stateu)
    #print(ultrasonic_state)
    
    # change state
    final_state = statechange(rf_state, ultrasonic_state)
    print("Final state: ")
    print(final_state)
    
    # toggles pins based on state
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
    #print("Output LEDs: ")
    #print(GPIO.input(out1))
    #print(GPIO.input(out0))
