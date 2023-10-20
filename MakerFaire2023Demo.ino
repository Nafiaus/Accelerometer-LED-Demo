/* Basic demo for Accelerometer and LED lights, made for the NWA Maker Faire 2023
    This could probably be optimized more, but I'm not that good at coding.
==========================================================================================
     NEOPIXEL BEST PRACTICES for most reliable operation:
     - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
     - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
     - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
     - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
     - connect GROUND (-) first, then +, then data.
==========================================================================================
Adafruit ID - Description
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    4438    - LSM6DSOX 6 DoF Accelerometer and Gyroscope
    2863    - NeoPixel Ring 24 GRBW ~6000K
    2856    - NeoPixel Ring 16 GRBW ~6000K
    2860    - NeoPixel Jewel 7 GRBW ~6000K
     367    - Tactile Button Switch
    ????    - Arduino Uno rev3
==========================================================================================
  You can get these items at Radioshack in Fayetteville (if your local, if not we do ship)
  If not in stock at the store you can also get them from adafruit.com
   - I don't believe Uno is on adafruits website. You'll have to get that elsewhere.
==========================================================================================
You will have to download "Adafruit LSM6DS", "Adafruit BusIO", "Adafruit Unitfied Sensor",
  and "Adafruit NeoPixel" libraries if you haven't already.
*/
#include <Adafruit_LSM6DSOX.h>
#include <Adafruit_NeoPixel.h>

// Change Pin and LED count if using something different
// You will have to change "updateLightPosition()" if you use different LED counts
#define OUTER_LED_PIN 11
#define OUTER_LED_COUNT 24
#define INNER_LED_PIN 12
#define INNER_LED_COUNT 16
#define JEWEL_LED_PIN 13
#define JEWEL_LED_COUNT 7

// For SPI mode, we need a CS pin
#define LSM_CS 10
// For software-SPI mode we need SCK/MOSI/MISO pins
#define LSM_SCK 9
#define LSM_MISO 8
#define LSM_MOSI 7

// If using Uno only use pin 2 or 3 since we are using internal pullup and interrupts
#define BUTTON_PIN 2

// Change Brightness, Wait, Time, and Debug status if wanted
#define BRIGHTNESS 25 // Max is 255, but is VERY bright
#define WAIT 50
#define TIME 25 // This is only for theaterChaseRainbow()
#define DEBUG_SERIAL true
#define DEBUG_LIGHTS false

// Colors for ease of use
#define BLUE Color(0,0,255)
#define GREEN Color(0,255,0)
#define RED Color(255,0,0)


typedef Adafruit_NeoPixel NeoPixel;

NeoPixel outer(OUTER_LED_COUNT, OUTER_LED_PIN, NEO_GRBW + NEO_KHZ800);
NeoPixel inner(INNER_LED_COUNT, INNER_LED_PIN, NEO_GRBW + NEO_KHZ800);
NeoPixel jewel(JEWEL_LED_COUNT, JEWEL_LED_PIN, NEO_GRBW + NEO_KHZ800);
NeoPixel strips[3] = {outer, inner, jewel};
Adafruit_LSM6DSOX sox;

uint32_t jColor = 0;
int colorState = 0;
unsigned int loopCount = 0;

void setup(void) {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(0, updateState, FALLING);

  Serial.begin(115200);
  while (!Serial)
    delay(10);
  if(DEBUG_SERIAL == true) {
    Serial.println();
    Serial.println("Adafruit LSM6DSOX test!");
  }

/* Comment out below line and uncomment the !sox line you need if this doesnt work */
  if (!sox.begin_I2C()) { // I2C Mode
    // if (!sox.begin_SPI(LSM_CS)) { // SPI Mode
    // if (!sox.begin_SPI(LSM_CS, LSM_SCK, LSM_MISO, LSM_MOSI)) { // software-SPI Mode
    if(DEBUG_SERIAL == true)
      Serial.println("Failed to find LSM6DSOX chip");
    while (1) {
      delay(10);
    }
  }
  
  // Change range to suit needs
  sox.setAccelRange(LSM6DS_ACCEL_RANGE_8_G);
  // Change range to suit needs
  sox.setGyroRange(LSM6DS_GYRO_RANGE_125_DPS);
  // Change rate to suit needs
  sox.setAccelDataRate(LSM6DS_RATE_12_5_HZ);
  // Change rate to suit needs
  sox.setGyroDataRate(LSM6DS_RATE_12_5_HZ);

  if(DEBUG_SERIAL == true) {
    Serial.println("LSM6DSOX Found!");
    Serial.print("Accelerometer range set to: ");
    switch (sox.getAccelRange()) {
    case LSM6DS_ACCEL_RANGE_2_G:
      Serial.println("+-2G");
      break;
    case LSM6DS_ACCEL_RANGE_4_G:
      Serial.println("+-4G");
      break;
    case LSM6DS_ACCEL_RANGE_8_G:
      Serial.println("+-8G");
      break;
    case LSM6DS_ACCEL_RANGE_16_G:
      Serial.println("+-16G");
      break;
    }

    Serial.print("Gyro range set to: ");
    switch (sox.getGyroRange()) {
    case LSM6DS_GYRO_RANGE_125_DPS:
      Serial.println("125 degrees/s");
      break;
    case LSM6DS_GYRO_RANGE_250_DPS:
      Serial.println("250 degrees/s");
      break;
    case LSM6DS_GYRO_RANGE_500_DPS:
      Serial.println("500 degrees/s");
      break;
    case LSM6DS_GYRO_RANGE_1000_DPS:
      Serial.println("1000 degrees/s");
      break;
    case LSM6DS_GYRO_RANGE_2000_DPS:
      Serial.println("2000 degrees/s");
      break;
    case ISM330DHCX_GYRO_RANGE_4000_DPS:
      break; // unsupported range for the DSOX
    }

    Serial.print("Accelerometer data rate set to: ");
    switch (sox.getAccelDataRate()) {
    case LSM6DS_RATE_SHUTDOWN:
      Serial.println("0 Hz");
      break;
    case LSM6DS_RATE_12_5_HZ:
      Serial.println("12.5 Hz");
      break;
    case LSM6DS_RATE_26_HZ:
      Serial.println("26 Hz");
      break;
    case LSM6DS_RATE_52_HZ:
      Serial.println("52 Hz");
      break;
    case LSM6DS_RATE_104_HZ:
      Serial.println("104 Hz");
      break;
    case LSM6DS_RATE_208_HZ:
      Serial.println("208 Hz");
      break;
    case LSM6DS_RATE_416_HZ:
      Serial.println("416 Hz");
      break;
    case LSM6DS_RATE_833_HZ:
      Serial.println("833 Hz");
      break;
    case LSM6DS_RATE_1_66K_HZ:
      Serial.println("1.66 KHz");
      break;
    case LSM6DS_RATE_3_33K_HZ:
      Serial.println("3.33 KHz");
      break;
    case LSM6DS_RATE_6_66K_HZ:
      Serial.println("6.66 KHz");
      break;
    }

    Serial.print("Gyro data rate set to: ");
    switch (sox.getGyroDataRate()) {
    case LSM6DS_RATE_SHUTDOWN:
      Serial.println("0 Hz");
      break;
    case LSM6DS_RATE_12_5_HZ:
      Serial.println("12.5 Hz");
      break;
    case LSM6DS_RATE_26_HZ:
      Serial.println("26 Hz");
      break;
    case LSM6DS_RATE_52_HZ:
      Serial.println("52 Hz");
      break;
    case LSM6DS_RATE_104_HZ:
      Serial.println("104 Hz");
      break;
    case LSM6DS_RATE_208_HZ:
      Serial.println("208 Hz");
      break;
    case LSM6DS_RATE_416_HZ:
      Serial.println("416 Hz");
      break;
    case LSM6DS_RATE_833_HZ:
      Serial.println("833 Hz");
      break;
    case LSM6DS_RATE_1_66K_HZ:
      Serial.println("1.66 KHz");
      break;
    case LSM6DS_RATE_3_33K_HZ:
      Serial.println("3.33 KHz");
      break;
    case LSM6DS_RATE_6_66K_HZ:
      Serial.println("6.66 KHz");
      break;
    }
  }
  for(int i = 0; i <= 3; i++) {
    strips[i].begin();
    strips[i].show();
    strips[i].setBrightness(BRIGHTNESS);
  }

  if(DEBUG_LIGHTS == true) {
    for(int i = 0; i <= 2; i++) {
      rainbow(&strips[i], WAIT);
      theaterChase(&strips[i], strips[i].RED, WAIT);
      theaterChase(&strips[i], strips[i].BLUE, WAIT);
      theaterChase(&strips[i], strips[i].GREEN, WAIT);
      theaterChaseRainbow(WAIT, TIME, &strips[i]);
    }
  }
}

void loop() {
  if(DEBUG_SERIAL == true) {
    Serial.println();
    Serial.print("\t\tLoop Count: ");
    Serial.println(loopCount, DEC);
    Serial.print("\t\tCurrent Color State: ");
    Serial.println(colorState, DEC);
  }
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  sox.getEvent(&accel, &gyro, &temp);

  if(jColor >= 65536) {
    jColor = 0;
  } else {
    jColor += 1024;
  }

  if(DEBUG_SERIAL == true) {
    Serial.print("\t\tJewel Color HSV: ");
    Serial.println(jColor, DEC);
  }
  updateLightPosition(&outer, accel, 5);
  updateTemperature(&inner, temp, 25);
  colorWipe(&jewel, jewel.ColorHSV(jColor), 25);
  delay(10);
  loopCount++;
}

// This function will update the colorState which will change the colors for "updateLightPosition()"
void updateState() {  
  if(colorState == 2) {
    colorState = 0;
  } else {
    colorState += 1;
  }
  if(DEBUG_SERIAL == true) {
    Serial.println("\t\tINTERRUPT: Button Pressed");
  }
}

// This function will change the strip colors based  on the accelerometer data.
void updateLightPosition(NeoPixel *strip, sensors_event_t accel, int wait) {
  if(DEBUG_SERIAL == true) {
    Serial.print("\t\tAccel X: ");
    Serial.print(accel.acceleration.x);
    Serial.print(" \tY: ");
    Serial.println(accel.acceleration.y);
  }
  for(int i = 0; i < 6; i++) {
    if(accel.acceleration.x < -3) {
      switch (colorState) {
        case 0:
          strip->setPixelColor(i, strip->RED);
          break;
        case 1:
          strip->setPixelColor(i, strip->GREEN);
          break;
        default:
          strip->setPixelColor(i, strip->BLUE);
          break;
      }
    } else if(accel.acceleration.x > 3) {
      switch (colorState) {
        case 0:
          strip->setPixelColor(i, strip->GREEN);
          break;
        case 1:
          strip->setPixelColor(i, strip->BLUE);
          break;
        default:
          strip->setPixelColor(i, strip->RED);
          break;
      }
    } else {
      switch (colorState) {
        case 0:
          strip->setPixelColor(i, strip->BLUE);
          break;
        case 1:
          strip->setPixelColor(i, strip->RED);
          break;
        default:
          strip->setPixelColor(i, strip->GREEN);
          break;
      }
    }
  }
  for(int i = 6; i < 12; i++) {
    if(accel.acceleration.y > 3) {
      switch (colorState) {
        case 0:
          strip->setPixelColor(i, strip->RED);
          break;
        case 1:
          strip->setPixelColor(i, strip->GREEN);
          break;
        default:
          strip->setPixelColor(i, strip->BLUE);
          break;
      }
    } else if(accel.acceleration.y < -3) {
      switch (colorState) {
        case 0:
          strip->setPixelColor(i, strip->GREEN);
          break;
        case 1:
          strip->setPixelColor(i, strip->BLUE);
          break;
        default:
          strip->setPixelColor(i, strip->RED);
          break;
      }
    } else {
      switch (colorState) {
        case 0:
          strip->setPixelColor(i, strip->BLUE);
          break;
        case 1:
          strip->setPixelColor(i, strip->RED);
          break;
        default:
          strip->setPixelColor(i, strip->GREEN);
          break;
      }
    }
  }
  for(int i = 12; i < 18; i++) {
    if(accel.acceleration.x < -3) {
      switch (colorState) {
        case 0:
          strip->setPixelColor(i, strip->GREEN);
          break;
        case 1:
          strip->setPixelColor(i, strip->BLUE);
          break;
        default:
          strip->setPixelColor(i, strip->RED);
          break;
      }
    } else if(accel.acceleration.x > 3) {
      switch (colorState) {
        case 0:
          strip->setPixelColor(i, strip->RED);
          break;
        case 1:
          strip->setPixelColor(i, strip->GREEN);
          break;
        default:
          strip->setPixelColor(i, strip->BLUE);
          break;
      }
    } else {
      switch (colorState) {
        case 0:
          strip->setPixelColor(i, strip->BLUE);
          break;
        case 1:
          strip->setPixelColor(i, strip->RED);
          break;
        default:
          strip->setPixelColor(i, strip->GREEN);
          break;
      }
    }
  }
  for(int i = 18; i < 24; i++) {
    if(accel.acceleration.y < -3) {
      switch (colorState) {
        case 0:
          strip->setPixelColor(i, strip->RED);
          break;
        case 1:
          strip->setPixelColor(i, strip->GREEN);
          break;
        default:
          strip->setPixelColor(i, strip->BLUE);
          break;
      }
    } else if(accel.acceleration.y > 3) {
      switch (colorState) {
        case 0:
          strip->setPixelColor(i, strip->GREEN);
          break;
        case 1:
          strip->setPixelColor(i, strip->BLUE);
          break;
        default:
          strip->setPixelColor(i, strip->RED);
          break;
      }
    } else {
      switch (colorState) {
        case 0:
          strip->setPixelColor(i, strip->BLUE);
          break;
        case 1:
          strip->setPixelColor(i, strip->RED);
          break;
        default:
          strip->setPixelColor(i, strip->GREEN);
          break;
      }
    }
  }
  strip->setBrightness(BRIGHTNESS);
  strip->show();
  delay(wait);
}

// This function will update the strip colors based on the temperature of the board
void updateTemperature(NeoPixel *strip, sensors_event_t temp, int wait) {
  // The board gives temp in C, but I'm AMERICAN and I don't use Communist units
  // (I'm kidding about Communist units, I just wanted to show how to convert from C to F)
  float degrees = ((1.8*temp.temperature)+32);
  /* This next line can use some explaining.
  32768 is half of 65536 (HSV max hue value) and is the Cyan color
  75 is the temperature in F that I want set as the lowest temp and the offset.
  2150 is the amount we will multiply with degrees to get our HSV hue.
    - 90(max temp read) - 70(low temp read) = 20(temp difference to read)
    - 85(max temp read) - 75(low temp read) = 10(temp difference to read)
    - Then 32768/20 to get 1638, and 32768/10 to get 3278.
    - 1638 was too slow on color change and 3278 was too fast for me,
    - so I kept choosing different values inbetween 1628-3278 until I found
    - a change of color/speed that I liked.

  [MAX COLOR HUE] - (([TEMP IN F] - [LOW TEMP READ]) * [HUE OFFSET]) = HSV Color 
  */
  uint32_t color = strip->ColorHSV((int)(32768-((degrees-75)*2150)));
  if(DEBUG_SERIAL == true) {
    Serial.print("\t\tTemperature (F): ");
    Serial.println(((1.8*temp.temperature)+32));
    Serial.print("\t\tTemperature (C): ");
    Serial.println(temp.temperature, DEC-8); // DEC-8 Removes 8 spots after the dot on floats
  }
  for(int i = 0; i < strip->numPixels(); i++) {
    strip->setPixelColor(i, color);
    strip->setBrightness(BRIGHTNESS);
    strip->show();
    delay(wait);
  }
}

// This function will change all LEDs to one color
void colorWipe(NeoPixel *strip, uint32_t color, int wait) {
  for(int i=0; i<strip->numPixels(); i++) {
    strip->setPixelColor(i, color);
    strip->setBrightness(BRIGHTNESS);
    strip->show();
    delay(wait);
  }
}

// This function will "chase" leds around at a certain color
void theaterChase(NeoPixel *strip, uint32_t color, int wait) {
  for(int a=0; a<10; a++) {
    for(int b=0; b<3; b++) {
      strip->clear();
      for(int c=b; c<strip->numPixels(); c += 3) {
        strip->setPixelColor(c, color);
      }
      strip->setBrightness(BRIGHTNESS);
      strip->show();
      delay(wait);
    }
  }
}

// This function will change all LEDs color in a rainbow effect
void rainbow(NeoPixel *strip, int wait) {
  for(long firstPixelHue = 0; firstPixelHue < 1*65536; firstPixelHue += 512) {
    strip->rainbow(firstPixelHue);
    strip->setBrightness(BRIGHTNESS);
    strip->show();
    delay(wait);
  }
}

// This function will "chase" the rainbow ;)
void theaterChaseRainbow(int wait, int time, NeoPixel *strip) {
  int firstPixelHue = 0;
  for(int a=0; a<time; a++) {
    for(int b=0; b<3; b++) {
      strip->clear();
      for(int c=b; c<strip->numPixels(); c += 3) {
        int      hue   = firstPixelHue + c * 65536L / strip->numPixels();
        uint32_t color = strip->gamma32(strip->ColorHSV(hue));
        strip->setPixelColor(c, color);
      }
      strip->setBrightness(BRIGHTNESS);
      strip->show();
      delay(wait);
      firstPixelHue += 65536 / 90;
    }
  }
  for(int i = 0; i < strip->numPixels(); i++) {
    strip->clear();
  }
  strip->show();
}
