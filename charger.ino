/* Author: Seth Shaw
 * Date: June 16, 2020
 * Description: shows different led effects based on voltage draw
 */
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define LED_PIN    5//output pin

#define LED_COUNT 27//num of leds

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

  Serial.begin(9600);
}

  uint32_t green=strip.Color(0, 255, 0);  //define led colours
  uint32_t blue=strip.Color(0, 0, 255);
  uint32_t off=strip.Color(0, 0, 0);
  int range=400;  //min voltage level when charging
  
bool charging(){  //returns true when charging
  return analogRead(A0)>range;
}

bool sleeping(){  //returns true when not charging
  return !charging();
}

bool True(){  //returns true
  return true;
}

/* sets all leds to 'colour'
 * delays 'duration' after changin each pixel
 * 'killTrail" gives the option to turn off trailing leds
 * accepts an abort function that wipes the stripe and returns
 */
void wipe(uint32_t colour, int duration, bool killTrail, bool(*func)()){
  for(int i=0; i<strip.numPixels(); i++) {  //loop through all pixels
    if(!(*func)()){
      wipe(off, 0, false, &True); //wipes stripe and aborts if func returns false
      return;
    }
    strip.setPixelColor(i, colour); //set next pixel colour
    if(killTrail)
      strip.setPixelColor(i-2, off);  //turns off trailing led if option selected
    strip.show(); //updates stripe
    delay(duration);
  }
}

void loop() {
  Serial.println(analogRead(A0)); //use serial monitor to set range
  
  int i, sec=59;
    
  while(analogRead(A0)>range){     //charging loop
    wipe(green, 100, true, &charging);  //loops green killing trail
    strip.setPixelColor(strip.numPixels()-1, off);  //kills remaining two LEDS
    strip.setPixelColor(strip.numPixels()-2, off);
    strip.show();
    sec=59;
  }
    
  while(analogRead(A0)<=range){    //sleep loop
    if(sec!=59){  //only runs once a minute
      break;
    }
    wipe(blue, 100, false, &sleeping);  //sets all leds blue
    wipe(off, 100, false, &sleeping);   //turns off all leds
    sec=0;  //set clock back to zero
  }
  delay(1000);
  sec++;
}
