#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
#include <WiFi.h>
#include "time.h"
#include <TFT_eSPI.h> // For TTGO T4 display
#define BLACK 0x0000
#define WHITE 0xFFFF
#define GREY  0x5AEB

#define WIFI_TIMEOUT  10 //seconds
bool WIFI_IS_CONNECTED = false;

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

int16_t h = 240;
int16_t w = 320;
int dly = 5;
bool is_black = true;
int factor = 10;

int micPin = 19;
int micValue;

constexpr int BARS_COUNT = 80; // w/4
int fr_bars[BARS_COUNT] = {0};
int current_bars[BARS_COUNT] = {0};
int GAP = 4;
int GAUSS_WIDTH = 3;
int GAUSS_DECREASE = 8;
int MAX_BAR = 70;

// WIFI settings
const char* ssid = "YotamZoey";
const char* password = "0545415851";
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7200;         // UTC+2 for Israel Standard Time
const int daylightOffset_sec = 3600;     // Add 1 hour for daylight saving time
struct tm timeinfo;
struct tm current_timeinfo;


void draw_lines(int factor){
    if (factor == 0){
      // tft.fillScreen(BLACK);
      tft.fillRect(0,21,w,h-21,BLACK);
      tft.fillRect(0,h/2,w,1,GREY);
    }
    for(int i=0; i< w; i++){
    if (i%4 == 0){
      int mult = (i/4)%6;
      tft.fillRect(i,h/2 - (factor*mult)/2,2,factor*mult,WHITE);
    }
  }
}


void bucket_filler(){
  for(int i=0; i< BARS_COUNT; i++ ){
    if (current_bars[i]){ // if this bar was marked

      for(int j = 0; j< GAUSS_WIDTH ; j++){
        // fill bars to it's right
        if (j+i < BARS_COUNT ){
          fr_bars[i+j] = fr_bars[i] - GAUSS_DECREASE*j;
        }

        // fill bars to it's left
          if (i-j >= 0 ){
          fr_bars[i-j] = fr_bars[i] - GAUSS_DECREASE*j;
        }
      }
    }
  }
}


void random_bucket_selection(){
  for(int i=0; i< BARS_COUNT; i++ ){
    if( (i > 10 && i < 40) || (i >60 && i < 75 )   ){
      fr_bars[i] =  random(MAX_BAR/2, MAX_BAR + 1);
    }
    else{
      fr_bars[i] =  random(0, MAX_BAR/2 + 1);
    }
      if (fr_bars[i] > 0 && i + GAP < BARS_COUNT -1 ){
        current_bars[i] = 1;
        i += GAP;
      }
    
  }
}


void dim_bucket(){
  for (int j = 0; j< 3; j++){
    // tft.fillScreen(BLACK);
    tft.fillRect(0,41,w,h-41,BLACK);
    tft.fillRect(0,h/2,w,1,GREY);
    
    for(int i=0; i< BARS_COUNT; i++ ){
      fr_bars[i] = fr_bars[i] - fr_bars[i]/4 ;
      tft.fillRect(i*4,h/2 - (fr_bars[i])/2,2,fr_bars[i],WHITE);
    }
    
    delay(30*(3-j));
    // delay(70);

  }
}

void clear_buckets(){
  for(int i=0; i< BARS_COUNT; i++ ){
    fr_bars[i] =  0;
    current_bars[i] = 0;
  }
}


void setup_wifi(){
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
}

void display_time(){
  if (getLocalTime(&current_timeinfo)) {
    if(current_timeinfo.tm_hour != timeinfo.tm_hour || current_timeinfo.tm_min != timeinfo.tm_min){
      getLocalTime(&timeinfo);
      char timeStr[16];
      strftime(timeStr, sizeof(timeStr), "%H:%M", &timeinfo);
      tft.fillRect(0,0,w,41,BLACK);
      tft.fillRect(0,h/2,w,1,GREY);
      tft.setCursor(180, 20); // Adjust coordinates as needed
      tft.print("Time: ");
      tft.println(timeStr);
    }

  } else {
    Serial.println("Failed to obtain time");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(micPin, INPUT); // Configures the sound sensor pin as input
  randomSeed(esp_random()); // Seed the random generator with ESP32's hardware RNG
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  tft.fillRect(0,h/2,w,1,GREY);
  // setup_wifi();
}

void draw_bars(){
    for (int j =0; j< BARS_COUNT ; j++){
    // Serial.print(fr_bars[j]);
    // Serial.print(", ");
    tft.fillRect(j*4,h/2 - (fr_bars[j])/2,2,fr_bars[j],WHITE);

  }
  Serial.println();
  dim_bucket();
}

void loop() {

  // put your main code here, to run repeatedly:

  // factor = (factor + 1)%10;
  // draw_lines(factor);
  micValue = digitalRead(micPin);
  if (micValue)
  {
    random_bucket_selection();
    bucket_filler();
    draw_bars();

    // delay(300);
    clear_buckets();
    tft.fillRect(0,41,w,h-41,BLACK);
    tft.fillRect(0,h/2,w,1,GREY);
  }
  else if(WIFI_IS_CONNECTED) {
    display_time();
  }
}
