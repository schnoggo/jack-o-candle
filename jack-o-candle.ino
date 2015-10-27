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
#define SCALERVAL 256*3
#define D_ false

// console buttons:
struct flame_element{
  int brightness;
  int step;
  int max_brightness;
  long rgb[3];
  byte state;
  } flames[5];
  
  int new_brightness = 0;
  unsigned long rgb[3]; //reusable temporary array
  uint8_t scaleD_rgb[3];
  byte acc;
  
 const int flamecolors[22][3] = {
 
{ SCALERVAL, 0,  0},
{ SCALERVAL, 0,  0},
{ SCALERVAL, 0,  0},
{ SCALERVAL, 0,  0},
{ SCALERVAL, 0,  0},
{ SCALERVAL, 0,  0},
{ SCALERVAL, 0,  0},
{ SCALERVAL, 0,  0},
{ SCALERVAL, SCALERVAL*.4,  },
{ SCALERVAL, SCALERVAL*.4,  0},
{ SCALERVAL, SCALERVAL*.4,  0},
{ SCALERVAL, SCALERVAL*.4,  0},
{ SCALERVAL, SCALERVAL*.3,  0},
{ SCALERVAL, SCALERVAL*.3,  0},
{ SCALERVAL, SCALERVAL*.3,  0},
{ SCALERVAL, SCALERVAL*.3,  0},
{ SCALERVAL, SCALERVAL*.3,  0},
{ SCALERVAL, SCALERVAL*.3,  0},
{ SCALERVAL, SCALERVAL*.3,  },
{ SCALERVAL, SCALERVAL*.3,  SCALERVAL}, // white
{ 0, SCALERVAL*.2,  SCALERVAL}, // that one blue flame
{ SCALERVAL,  SCALERVAL*.3,  SCALERVAL*.5}
};

const int speeds[30] ={ 
    1,1,1,2,2,2,3,3,3,4,
    4,4,4,4,4,5,5,5,5,5,
    6,6,6,7,7,8,8,10,10,20
    };
  
  
  
void setup() {
  if (D_){
    Serial.begin(9600);
    Serial.println("STARTUP");
  }
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  randomSeed(analogRead(4));
  InitFlames();
  
}

void loop() {

  for(byte i=0; i<NUMBER_OF_FLAMES; i++) {
    switch(flames[i].state){
      case 0: // reset
      CreateNewFlame(i);
      break;
      
      case 1: //increasing
        new_brightness = flames[i].brightness + flames[i].step;
        if (new_brightness > flames[i].max_brightness){
          flames[i].state = 2;
        } else {
          UpdateFlameColor(i, new_brightness);
          flames[i].brightness = new_brightness;
        }
      
      break;
      
      
      case 2: //decreasing
        new_brightness = flames[i].brightness - flames[i].step;

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
   delay(29);
  
} //loop()


void InitFlames(){
// Sets initial states in flames array
  for(byte i=0; i<NUMBER_OF_FLAMES; i++) {
    flames[i].state=0;
    
  }
}


void UpdateFlameColor(byte flame_num, int new_brightness){
// 
  uint32_t c = 0;
  new_brightness = min(new_brightness, flames[flame_num].max_brightness);
  uint32_t color_channel_value;
  if ((D_) && (0 == flame_num)){  Serial.print("pre:( ");}

  for(byte i=0; i<3; i++) {
    color_channel_value = flames[flame_num].rgb[i];
    if ((D_) && (0 == flame_num)){ Serial.print(color_channel_value); }
    color_channel_value = color_channel_value * new_brightness; // keep it long
    color_channel_value = color_channel_value/SCALERVAL;
    rgb[i] = max(0L,color_channel_value);
    if ((D_) && (0 == flame_num)){
      Serial.print("-"); 
      Serial.print(rgb[i]); 
      Serial.print(", ");
    }
  }

  if ((D_) && (0 == flame_num)){
    Serial.print(") bright:");
    Serial.print(new_brightness);
    Serial.print("/");
    Serial.print(flames[flame_num].max_brightness);
    Serial.print("  ");
  }


  // this version just divides 'em up: (Eventually get higher precision using all three)
  if ((D_) && (0 == flame_num)){
    Serial.print("scaled: ");
    Serial.print(" (");
  }

  for(byte sub_pixel=0; sub_pixel<3; sub_pixel++) {
    for(byte i=0; i<3; i++) { // rgb
      acc = rgb[i]/3;
      byte d = rgb[i]%3;
      if (sub_pixel < d){
        acc ++;
      }
      scaleD_rgb[i] = acc;
      
    }
    c = strip.Color(scaleD_rgb[0],scaleD_rgb[1], scaleD_rgb[2]);
    strip.setPixelColor(flame_num*3 + sub_pixel, c);
  }
  
}


void CreateNewFlame(byte flame_num){

  flames[flame_num].step = speeds[random(30)];
  flames[flame_num].max_brightness = SCALERVAL/(random(5)+1);
//  flames[flame_num].max_brightness = random(SCALERVAL/4) +  random(SCALERVAL/4) + random(SCALERVAL/4) + SCALERVAL/4 +1;
  flames[flame_num].brightness = 0;
  flames[flame_num].state = 1;
  byte color_index = random(22);
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


