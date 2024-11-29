
#define BLACK 0x0000
#define WHITE 0xFFFF
#define GREY  0x5AEB

#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

int16_t h = 240;
int16_t w = 320;
int dly = 5;
bool is_black = true;
int factor = 10;


void draw_lines(int factor){
    if (factor == 0){
      tft.fillScreen(BLACK);
      tft.fillRect(0,h/2,w,1,GREY);
    }
    for(int i=0; i< w; i++){
    if (i%4 == 0){
      int mult = (i/4)%6;
      tft.fillRect(i,h/2 - (factor*mult)/2,2,factor*mult,WHITE);
    }
  }
}



void setup() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  tft.fillRect(0,h/2,w,1,GREY);
}

void loop() {

  // put your main code here, to run repeatedly:
  delay(500);
  factor = (factor + 1)%10;
  draw_lines(factor);

}
