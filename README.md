# Accelerometer-LED-Demo
Basic demo for Accelerometer and LED lights, made for the NWA Maker Faire 2023 I don't know
much about coding, so this code could probably use some refinements, but this isn't designed
to be mass produced. It was just meant as a project for ya'll to put together without having
to create all the code from scratch.

==========================================================================================
Adafruit ID - Description
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    4438    - LSM6DSOX 6 DoF Accelerometer and Gyroscope
    2863    - NeoPixel Ring 24 RGBW ~6000K
    2856    - NeoPixel Ring 16 RGBW ~6000K
    2860    - NeoPixel Jewel 7 RGBW ~6000K
     367    - Tactile Button Switch
    ????    - Arduino Uno rev3
==========================================================================================
  You can get these items at Radioshack in Fayetteville (if your local, if not we do ship)
  If not in stock at the store you can also get them from adafruit.com
   - I don't believe Uno is on adafruits website. You'll have to get that elsewhere.
==========================================================================================

You will have to download libraries if you haven't already.
- "Adafruit LSM6DS"
- "Adafruit BusIO"
- "Adafruit Unitfied Sensor",
- "Adafruit NeoPixel" 

Most NeoPixel LEDs are going to be GRB(W), so if you use different LEDs make sure to change
the mode of the LEDs. Also using different LEDs with different # of LEDs will cause you to
have to change code in some of the functions.
