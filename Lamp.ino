#include <Adafruit_NeoPixel.h>

#define PIN 6 //pixel data line
#define PANELPIXELCOUNT 4 //how many pixels per panel
#define NUMPANELS 1 //number of seperate panels on strip
#define NUMPIXELS 4 //total number of pixels total on strip


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int delayTime = 15; // 0.015 secconds

void setup(){
  pixels.begin(); //initializes NP library
}

void loop(){
  //Main boot loop function
  int mode = 0;//use analog input to change modes
  if(mode == 0){
    initSet(mode);
    pixels.show();
    while(mode == 0){
      solidPanelRainbow();
      pixels.show();
      delay(delayTime);
    }
  }
}

//using all pixels in project on single strip you can section them off evenly to create seperate panels
//this function uses the first pixel of the panel as the refrence for entire panel
//effect is that you can rainbow cycle entire strip of pannels while keeping the panels diferent colors with proper initialisation
void solidPanelRainbow(){
  for(int p = 0; p < NUMPANELS; p++){
    uint32_t c = 0;
    c = pixels.getPixelColor( (p*PANELPIXELCOUNT) );//get 32bit packed pixel from first pixel in pannel
    
    uint8_t r = (c>>16) &0xff;//unpack 8-bit segments into the r/g/b parts
    uint8_t g = (c>>8) &0xff;
    uint8_t b = c &0xff;
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
    
    setPanel(p,r,g,b);
  }
}

void initSet(int modeSelection){
  switch(modeSelection){
    case 0://solidRainbow
      setPanel(0,0,0,255);
      setPanel(1,0,255,127);
      setPanel(2,255,255,0);
      setPanel(3,255,0,127);
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
