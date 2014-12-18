#include <Adafruit_NeoPixel.h>

#define PIN 6 //pixel data line
#define PANELPIXELCOUNT 4 //how many pixels per panel
#define NUMPANELS 1 //number of seperate panels on strip
#define NUMPIXELS 4 //total number of pixels total on strip


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int delayTime = 15; // 0.015 secconds
int mode = 0;
int val = 0;
uint8_t level = 0;

void setup(){
  pixels.begin(); //initializes NP library
  pixels.show();
  Serial.begin(9600);
}

void loop(){
  //Main boot loop function
  mode = modeSelect();
  switch(mode){
    case 0://white lamp mode
      val = analogRead(6);
      level = val / 4;
      for(int i = 0; i < NUMPIXELS; i++){
        pixels.setPixelColor(i,level,level,level);
      }
      pixels.show();
      while(mode == 0){
        val = analogRead(6);
        level = val / 4;
        for(int i = 0; i < NUMPIXELS; i++){
          pixels.setPixelColor(i,level,level,level);
        }
        pixels.show();
        mode = modeSelect();
      }
      break;
    case 1://4 pannels with even offset on color wheel
      initSet(1);
      pixels.show();
      while(mode == 1){
        solidPanelRainbow();
        pixels.show();
        delay(delayTime);
        mode = modeSelect();
      }
      break;
    case 2://all panels same color rainbow fade
      for(int i = 0; i < NUMPIXELS; i++){
        pixels.setPixelColor(i,0,0,0);
      }
      pixels.show();
      while(mode == 2){
        solidPanelRainbow();
        pixels.show();
        delay(delayTime);
        mode = modeSelect();
      }
      break;
    case 3://chase chain in panel
      initSet(3);
      pixels.show();
      while(mode == 3){
        solidPanelRainbowLead();
        pixels.show();
        delay(delayTime);
        mode = modeSelect();
      }
      break;
    case 4:
      pixels.setPixelColor(0,0,0,255);
      pixels.setPixelColor(1,16,16,255);
      pixels.setPixelColor(2,32,32,255);
      pixels.setPixelColor(3,64,64,255);
      pixels.show();
      while(mode == 4){
        uint32_t c0 = pixels.getPixelColor(0);
        uint8_t rg0 = (c0>>16) &0xff;
        uint32_t c1 = pixels.getPixelColor(1);
        uint8_t rg1 = (c1>>16) &0xff;
        uint32_t c2 = pixels.getPixelColor(2);
        uint8_t rg2 = (c2>>16) &0xff;
        uint32_t c3 = pixels.getPixelColor(3);
        uint8_t rg3 = (c3>>16) &0xff;
        pixels.setPixelColor(2,rg3,rg3,255);
        pixels.setPixelColor(3,rg0,rg0,255);
        pixels.setPixelColor(0,rg1,rg1,255);
        pixels.setPixelColor(1,rg2,rg2,255);
        pixels.show();
        delay(95);
        mode = modeSelect();
      }
      break;
    case 5:
    default:
      for(int i = 0; i < NUMPIXELS; i++){
        pixels.setPixelColor(i,0,0,0);
        pixels.show();
      }
      while(mode > 3){
        mode = modeSelect();
      }
    }
}

int modeSelect(){
  int A0 = analogRead(0);
  int A1 = analogRead(1);
  int A2 = analogRead(2);
  int A3 = analogRead(3);
  int A4 = analogRead(4);
  int A5 = analogRead(5);
  if(A0 == 1023){
    return 0;
  }else if(A1 == 1023){
    return 1;
  }else if(A2 == 1023){
    return 2;
  }else if(A3 == 1023){
    return 3;
  }else if(A4 == 1023){
    return 4;
  }else if(A5 == 1023){
    return 5;
  }
  return -1;
}

uint32_t rgbShift(uint32_t c){
    uint32_t r = (c>>16) &0xff;//unpack 8-bit segments into the r/g/b parts
    uint32_t g = (c>>8) &0xff;
    uint32_t b = c &0xff;
    //this patern allows for 1536 color resolution
    //increased resolution can be done after by a diming factor but this can be done at best 50% increase 
    //due to some colors needing a hint of tint of the primes eg. contrast changing
    //this pattern cycles thru blue>purple>red>yellow>green>teal>blue>...
    //(0,0,255)blue > (255,0,255)purple/pink > (255,0,0)red > (255,255,0)yellow > (0,255,0)green > (0,255,255)teal/cyan > (0,0,255)blue >
    if( b==(uint8_t)255 ){
      if(g > (uint8_t)0 ){
        g--;
      }else if( r < (uint8_t)255 ){
        r++;
      } else {
        b--;
      }
    }else if( r == (uint8_t)255 ){
      if( b > (uint8_t)0 ){
        b--;
      } else if( g < (uint8_t)255 ){
        g++;
      } else {
        r--;
      }
    }else{
      if( r > (uint8_t)0 ){
        r--;
      } else if(b < (uint8_t)255){
        b++;
      } else {
        g--;
      }
    }
    c = r<<16 | g<<8 |b ;
    return (c );
}

//using all pixels in project on single strip you can section them off evenly to create seperate panels
//this function uses the first pixel of the panel as the refrence for entire panel
//effect is that you can rainbow cycle entire strip of pannels while keeping the panels diferent colors with proper initialisation
void solidPanelRainbow(){
  for(int p = 0; p < NUMPANELS; p++){
    uint32_t c;
    c = pixels.getPixelColor( (p*PANELPIXELCOUNT) );//get 32bit packed pixel from first pixel in pannel
    c = rgbShift(c);
    uint8_t r = (c>>16) &0xff;//unpack 8-bit segments into the r/g/b parts
    uint8_t g = (c>>8) &0xff;
    uint8_t b = c &0xff;
    
    setPanel(p,r,g,b);
  }
}

void solidPanelRainbowLead(){
  for(int p = 0; p < NUMPIXELS; p++){
    uint32_t c;
    c = pixels.getPixelColor(p);//get 32bit packed pixel from first pixel in pannel
    c = rgbShift(c);
    uint8_t r = (c>>16) &0xff;//unpack 8-bit segments into the r/g/b parts
    uint8_t g = (c>>8) &0xff;
    uint8_t b = c &0xff;
    pixels.setPixelColor(p,r,g,b);
  }
}

void initSet(int modeSelection){
  switch(modeSelection){
    case 1://solidRainbow
      setPanel(0,0,0,255);
      setPanel(1,0,255,127);
      setPanel(2,255,255,0);
      setPanel(3,255,0,127);
      break;
    case 3:
      setPanelLead(0,0,0,255);
      setPanelLead(1,0,255,127);
      setPanelLead(2,255,255,0);
      setPanelLead(3,255,0,127);
      break;
  }
}

void setPanel(int i,uint8_t r,uint8_t g,uint8_t b){
  //i is panel 0,1,2,3,...
  //j is pixel increment
  //r,g,b are 8-bit values to set for each pixel in selected panel
  for( int j=0; j<PANELPIXELCOUNT; j++){
    pixels.setPixelColor( (i*PANELPIXELCOUNT + j), pixels.Color(r,g,b) );
  }
}

void setPanelLead(int i,uint8_t r,uint8_t g,uint8_t b){
  //i is panel 0,1,2,3,...
  //r,g,b are 8-bit values to set for lead pixel in selected panel
  //each pixel following lead will take the lead's last known color
  pixels.setPixelColor((i*PANELPIXELCOUNT), pixels.Color(r,g,b) );
  for(int j=1; j<PANELPIXELCOUNT ;j++){
    uint32_t c = pixels.getPixelColor(j-1+i*PANELPIXELCOUNT);//get 32bit packed pixel from first pixel in pannel
    for(int k = 0; k < 35; k++){
      c = rgbShift(c);
    }
    uint8_t rl = (c>>16) &0xff;//unpack 8-bit segments into the r/g/b parts
    uint8_t gl = (c>>8) &0xff;
    uint8_t bl = c &0xff;
    pixels.setPixelColor(j+i*PANELPIXELCOUNT, pixels.Color(rl,gl,bl) );
  }
}
