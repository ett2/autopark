# -*- coding: utf-8 -*-
"""
--------------------------------------------------------------------------
Pedometer on PocketBeagle using Pedometer 3 Click & SPI LCD Screen
--------------------------------------------------------------------------
Juan Garza
This code was modifed form of https://www.hackster.io/170850/standalone-pedometer-pocketbeagle-mikro-click-boards-d14d93
This code is for engi 301 and is not meant for monetary gain 
Credits to Jennifer Haller, Cathy Wicks, Greg Sheridan for the preios code which was modified
for the pedometer reading and sensor usage


https://www.hackster.io/jag33/engi-301-pedometer-313707

--------------------------------------------------------------------------
Software API:

  * Class Pedometer()
      - Contains all functions for interacting with the pedometer and display 
          click boards:
  * init()
      - Initializes the pedometer and configures/enables the step count engine
  * read_accel()
      - Reads 6 bytes of acceleration data
      - Returns x-, y-, z-acceleration for +/-2g range
  * read_steps()
      - Reads 2 bytes of step count data
      - Returns the integer value
 class BuzzerSound 
    -uses frequency and time the buzzer is on for alerting user
    
 

  * Class attributes:
      - state: tracks the current state of the device (INIT, WAIT, WALK, REST)
      - step_total: tracks the current step total for this exercise
      - time_counter: tracks the amount of time passing for task scheduling
      - sleep_num: the current frame of the sleep animation (1-3)
      - walk_num: the current frame of the walking animation (0-4)
  
  function SPI_display_text()
    -updates the display, uses all the data that will be displayed. 
  
--------------------------------------------------------------------------
Background Information: 
 
  * Using the PocketBeagle:
    * https://github.com/beagleboard/pocketbeagle/wiki/System-Reference-Manual

  * Using the Pedometer 3 Click board from MikroElektronika:
    * https://www.mikroe.com/pedometer-3-click
    * http://kionixfs.kionix.com/en/datasheet/KX126-1063-Specifications-Rev-2.0.pdf
    * http://kionixfs.kionix.com/en/document/AN073-Getting-Started-with-Pedometer.pdf
    * https://github.com/RohmSemiconductor/Arduino/tree/master/KX126

  * Using the OLED C Click board from MikroElektronika:
    * https://www.hackster.io/103416/standalone-magic-8-ball-pocketbeagle-mikro-click-boards-4f1bb4

  * Using the display
    ▪ https://learn.adafruit.com/adafruit-2-8-and-3-2-color-tft-touchscreen-breakout-v2/overview
    ▪ https://learn.adafruit.com/adafruit-2-8-and-3-2-color-tft-touchscreen-breakout-v2/spi-wiring-and-test
    ▪ https://learn.adafruit.com/adafruit-2-8-and-3-2-color-tft-touchscreen-breakout-v2/python-wiring-and-setup
    ▪ https://learn.adafruit.com/adafruit-2-8-and-3-2-color-tft-touchscreen-breakout-v2/python-usage

  * Using the GPIO library:
    * https://github.com/adafruit/Adafruit_Python_GPIO/tree/master/Adafruit_GPIO

--------------------------------------------------------------------------
"""
#for the origial pedometer
import time
import os
from PIL import Image, ImageDraw, ImageFont

#for the Display of the pedoneter
import digitalio
import board
from PIL import Image, ImageDraw, ImageFont
import adafruit_rgb_display.ili9341 as ili9341
import adafruit_rgb_display.st7789 as st7789  # pylint: disable=unused-import
import adafruit_rgb_display.hx8357 as hx8357  # pylint: disable=unused-import
import adafruit_rgb_display.st7735 as st7735  # pylint: disable=unused-import
import adafruit_rgb_display.ssd1351 as ssd1351  # pylint: disable=unused-import
import adafruit_rgb_display.ssd1331 as ssd1331  # pylint: disable=unused-import

# First define some constants to allow easy resizing of shapes.
BORDER = 20
FONTSIZE = 24
# Configuration for CS and DC pins (these are PiTFT defaults):
cs_pin = digitalio.DigitalInOut(board.P9_17)
dc_pin = digitalio.DigitalInOut(board.P9_15)
reset_pin = digitalio.DigitalInOut(board.P9_12)
# Config for display baudrate (default max is 24mhz):
BAUDRATE = 24000000
# Setup SPI bus using hardware SPI:
spi = board.SPI()


disp = ili9341.ILI9341(spi,
    rotation=90,  # 2.2", 2.4", 2.8", 3.2" ILI9341
    cs=cs_pin,
    dc=dc_pin,
    rst=reset_pin,
    baudrate=BAUDRATE,)
# pylint: enable=line-too-long
# Create blank image for drawing.
# Make sure to create image with mode 'RGB' for full color.
if disp.rotation % 180 == 90:
    height = disp.width  # we swap height/width to rotate it to landscape!
    width = disp.height
else:
    width = disp.width  # we swap height/width to rotate it to landscape!
    height = disp.height
image = Image.new("RGB", (width, height))

# Get drawing object to draw on image.
draw = ImageDraw.Draw(image)



# First define some constants to allow easy positioning of text.
padding = -2
#-2
x = 20

# Load a TTF font.  Make sure the .ttf font file is in the
# Some other nice fonts to try: http://www.dafont.com/bitmap.php
font = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24)



def SPI_display_text(tot_steps, avg_rat, time_counted, state, ins_rat, set_speed, all_steps,tot_time):
    '''
    Use for the display, every Time this Function is used, the screen is updates with the values
    '''
    draw.rectangle((0, 0, width, height), outline=0, fill=(233, 196, 106))
    y = padding
    time_str =      "Loop: " + str(time_counted)
    speedstr =      "Speed Goal SPM: " + str(set_speed)
    step_str =      "Cycle Steps: " + str(tot_steps)
    avg_state_str = "Avg Rate SPM: " + str(round(avg_rat,1))
    state_str =     "Code State: " + str(state)
    ins_rat_str =   "Inst. Rate SPM: " + str(ins_rat)
    all_steps_str = "All Steps: " + str(all_steps)
    
    tot_min = int(tot_time//60)
    tot_sec = round(tot_time%60,1)
    #print(tot_sec)
    t_time_str = "Time: 0" + str(tot_min)+":"+str(tot_sec)
    #to make time display much prettier
    
    draw.text((x, y), time_str, font=font, fill="#0000FF")
    y += font.getsize(time_str)[1]
    
    draw.text((x, y), t_time_str, font=font, fill="#023047")
    y += font.getsize(t_time_str)[1]
    y += font.getsize(time_str)[1]/2
   
    draw.text((x, y), speedstr, font=font, fill="#540b0e")
    y += font.getsize(speedstr)[1]
    
    draw.text((x, y), avg_state_str, font=font, fill="#333d29")
    y += font.getsize(avg_state_str)[1]
    
    draw.text((x, y), step_str, font=font, fill="#001219")
    y += font.getsize(step_str)[1]
    y += font.getsize(step_str)[1]/2
    #draw.text((x, y), state_str, font=font, fill="#540b0e")
    #y += font.getsize(state_str)[1]
    
    draw.text((x, y), ins_rat_str, font=font, fill="#0000FF")
    y += font.getsize(ins_rat_str)[1]
    
    draw.text((x, y), all_steps_str, font=font, fill="#F3500F")
    y += font.getsize(all_steps_str)[1]
    
    
    disp.image(image)



import time
import Adafruit_BBIO.GPIO as GPIO
import Adafruit_BBIO.ADC as ADC
import Adafruit_BBIO.PWM as PWM

'''GPIO.setup("P2_2", GPIO.IN)
GPIO.setup("P2_3", GPIO.IN)
GPIO.setup("P2_4", GPIO.IN)
GPIO.setup("P2_6", GPIO.IN)
#initialize the pins
'''



rate = 0
loop_num = 0
avg_rate = 0
set_speed = 0
cycle_steps = 0


#while True:

    #Time in the loop, updates the time on the pedemeter
'''
    #for each button and their uses
    if GPIO.input("P2_2") == GPIO.LOW:
        #blue button for increasing step speed goal
        set_speed += 20
    
    if GPIO.input("P2_3") == GPIO.LOW:
        #red button for decreasing step speed goal
        if set_speed > 0: 
            set_speed -= 10
        else: 
            pass
    if GPIO.input("P2_4") == GPIO.LOW:
        #green Button which resets speed 
        set_speed = 60
        buzzerfunc(300,.5)
        #buzzer to alert if it was pressed

    if GPIO.input("P2_6") == GPIO.LOW:
        #Yellow button which resets speed, time, but not all of the total steps made
        #loop_num = 0
        cycle_steps = 0
        start_time = time.time()
        #rests the time
        buzzerfunc(300,.5)
    else:
        pass
    
    if check_speed(set_speed, avg_rate):
        buzzerfunc(300,.5)#beeps if speed is far
'''
SPI_display_text(0, 0, 0, 0, 0, 0, 0, 0)   

'''
    accel = ped.read_accel()

    if (ped.state == INIT):
        # Configure the pedometer and initialize attributes
        ped.init()
        ped.sleep_num = 1
        ped.walk_num = 0
        ped.time_counter = 0
        ped.step_total = 0
        ped.state = WAIT
        continue
        
    elif (ped.state == WAIT):
        # Check for steps
        new_steps = ped.read_steps()
        if (new_steps > 0):
            ped.step_total += new_steps         # update the total before state change
            cycle_steps += new_steps #updates cycle steps as well
            ped.sleep_num = 1                   # reset this for next use
            ped.state = MOVE
            continue
        else:
            ped.sleep_num += 1                  # Cycle through images
            if (ped.sleep_num == 4):
                ped.sleep_num = 1
            
    elif(ped.state == MOVE):
        # If not moving, go to REST state
        if ((abs(accel[0]) < 2) and (abs(accel[1]) < 2) and (abs(accel[2]) < 2)):
            ped.time_counter = 0
            ped.state = REST
            continue
        
        else:
            # Increment time counter
            ped.time_counter += 1
            # Display next image in movement animation
            ped.walk_num += 1
            if (ped.walk_num == 5):
                ped.walk_num = 0
            
            # Every 3 seconds, read the step count
            if (ped.time_counter >= 3):
                new_steps = ped.read_steps()
                ped.step_total += new_steps
                cycle_steps += new_steps
                rate = new_steps/ped.time_counter *60/2
                # the instant rate is updated
                ped.time_counter = 0
                # If no new steps in 3 seconds, go to REST state
                if (new_steps == 0):
                    ped.state = REST
        
    elif(ped.state == REST):
        # Display total step count when entering REST state
        if (ped.time_counter == 0):
            new_steps = ped.read_steps()
            ped.step_total += new_steps
            cycle_steps += new_steps
        
        # Check for movement
        new_steps = ped.read_steps()
        if (new_steps > 0):
            ped.step_total += new_steps        # update the total before state change
            cycle_steps += new_steps
            ped.time_counter = 0               # reset this for next use
            
            ped.state = MOVE
            continue
        else:
            ped.time_counter += 1              # increment to track time in REST state
        
        # Reset after a certain amount of inactivity
        if (ped.time_counter >= 30):
            ped.time_counter = 0
            ped.state = WAIT
            continue
        
    else:   # Should never reach this
        ped.state = INIT
        continue
'''

        
