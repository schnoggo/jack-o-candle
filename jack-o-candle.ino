#include <Adafruit_NeoPixel.h>

#define PIN 2	

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, 2, NEO_GRB + NEO_KHZ800);

/*
 5 "flames" of 3 pixels each.
 Each flame can have a brightness of 0 to 254 (play with this scale)
 Eventually, light up center pixel of three first then the sides. This version will just distribute amongst the 3 pixels.

*/

#define NUMBER_OF_FLAMES 5
#define SCALERVAL 700

// console buttons:
struct flame_element{
  int brightness;
  int step;
  int max_brightness;
  int rgb[3];
  byte state;
  } flames[5];
  
  int new_brightness = 0;
  int rgb[3]; //reusable temporary array
  uint8_t scaled_rgb[3];
  
 const int flamecolors[12][3] = {
 /*
{ SCALERVAL, 0,  0},
{ SCALERVAL, 0,  0},
{ SCALERVAL, 0,  0},
{ SCALERVAL, 0,  0},
{ SCALERVAL, 0,  0},
{ SCALERVAL, 0,  0},
{ SCALERVAL, 0,  0},
{ SCALERVAL, 0,  0},
*/
{ SCALERVAL, SCALERVAL*.02,  },
{ SCALERVAL, SCALERVAL*.02,  0},
{ SCALERVAL, SCALERVAL*.02,  0},
{ SCALERVAL, SCALERVAL*.02,  0},
{ SCALERVAL, SCALERVAL*.02,  0},
{ SCALERVAL, SCALERVAL*.02,  0},
{ SCALERVAL, SCALERVAL*.02,  0},
{ SCALERVAL, SCALERVAL*.02,  0},
{ SCALERVAL, SCALERVAL*.02,  0},
{ SCALERVAL, SCALERVAL*.02,  0},
{ SCALERVAL, SCALERVAL*.02,  0},
{ SCALERVAL, SCALERVAL*.02,  0}
/*
{ SCALERVAL, SCALERVAL*.3,  0},
{ SCALERVAL, SCALERVAL*.3,  0},
{ SCALERVAL, SCALERVAL*.3,  0},
{ SCALERVAL, SCALERVAL*.3,  0},
{ SCALERVAL, SCALERVAL*.4,  SCALERVAL*.1},
{ SCALERVAL, SCALERVAL*.3,  0},
{ SCALERVAL, SCALERVAL*.3,  0},
{ SCALERVAL, 0,  0},
{ 0, SCALERVAL*.2,  SCALERVAL},
{ SCALERVAL*2., SCALERVAL*.4,  SCALERVAL},
{ SCALERVAL*.6,  SCALERVAL*.3,  SCALERVAL},
{ SCALERVAL,  SCALERVAL*.3,  SCALERVAL*.5}
*/
};

  
  
  
  
void setup() {
	Serial.begin(9600);
Serial.println("STARTUP");
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'å
  randomSeed(analogRead(4));
  InitFlames();
  
}

void loop() {
/*
  // Some example procedures showing how to display to the pixels:
  colorWipe(strip.Color(255, 0, 0), 50); // Red
  colorWipe(strip.Color(0, 255, 0), 50); // Green
  colorWipe(strip.Color(0, 0, 255), 50); // Blue
  rainbow(20);
  rainbowCycle(20);
  */
  
  for(byte i=0; i<NUMBER_OF_FLAMES; i++) {
    switch(flames[i].state){
      case 0: // reset
      CreateNewFlame(i);
      break;
      
      case 1: //increasing
        new_brightness = flames[i].brightness + flames[i].step;
        /*
        Serial.print(i );
        Serial.print("] old: ");
        Serial.print(flames[i].brightness );
        Serial.print(", new: ");
        Serial.print(new_brightness);
        Serial.print(", max: ");
        Serial.println(flames[i].max_brightness);  
        */
   
        if (new_brightness > flames[i].max_brightness){
          flames[i].state = 2;
        } else {
          UpdateFlameColor(i, new_brightness);
          flames[i].brightness = new_brightness;
        }
      
      break;
      
      
      case 2: //decreasing
        new_brightness = flames[i].brightness - flames[i].step;
        /*
        Serial.print(i );
        Serial.print("] old: ");
        Serial.print(flames[i].brightness );
        Serial.print(", new: ");
        Serial.println(new_brightness);
        */
 
        if (new_brightness <1){
          flames[i].state = 0; // bottomed out - reset to next flame
        } else {
          UpdateFlameColor(i, new_brightness);
           flames[i].brightness = new_brightness;
        }
    
      break;
      
    
    
    
    }
  
  }
   strip.show();
   delay(15);
  
} //loop


void InitFlames(){
  for(byte i=0; i<NUMBER_OF_FLAMES; i++) {
    flames[i].state=0;
    
  }
}


void UpdateFlameColor(byte flame_num, int new_brightness){
  uint32_t c = 0;
  int color_channel_value;
  for(byte i=0; i<3; i++) {
    color_channel_value = flames[flame_num].rgb[i];
    rgb[i] = (color_channel_value * SCALERVAL)/new_brightness;
  
  }
  // this version just divides 'em up:
  for(byte i=0; i<3; i++) {
    scaled_rgb[i] = rgb[i]/3; // max this at some point
  }
  

  c = strip.Color(scaled_rgb[0],scaled_rgb[1], scaled_rgb[2]);
  for(byte i=flame_num*3; i<(flame_num*3)+3; i++) {
    strip.setPixelColor(i, c);
  }


}


void CreateNewFlame(byte flame_num){

  flames[flame_num].step = random(12)+1;
  flames[flame_num].max_brightness = SCALERVAL/(random(4)+1);
  flames[flame_num].brightness = 0;
  flames[flame_num].state = 1;
  byte color_index = random(12);
   for(byte i=0; i<3; i++) {
     flames[flame_num].rgb[i] = flamecolors[color_index][i];
   }
   /*
   Serial.println("New Flame");
   Serial.print("step: ");
   Serial.println(flames[flame_num].step);
    Serial.print("max_brightness: ");
     Serial.println(flames[flame_num].max_brightness);  
    Serial.print("color: (");
     Serial.print(flames[flame_num].rgb[0]);
      Serial.print(", ");
       Serial.print(flames[flame_num].rgb[1]);
        Serial.print(", ");
        Serial.print(flames[flame_num].rgb[2]);
        Serial.println(") ");
  
   */
}



// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

