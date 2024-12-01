#include <WiFi.h>
#include "time.h"
#include <TFT_eSPI.h> // For TTGO T4 display

TFT_eSPI tft = TFT_eSPI(); // Initialize display

const char* ssid = "YotamZoey";
const char* password = "0545415851";

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 3600;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  tft.init();
  tft.setRotation(1); // Adjust if needed for display orientation
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
}

void loop() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    char timeStr[16];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);

    tft.fillScreen(TFT_BLACK); // Clear the screen
    tft.setCursor(180, 20); // Adjust coordinates as needed
    tft.print("Time: ");
    tft.println(timeStr);
  } else {
    Serial.println("Failed to obtain time");
  }
  delay(1000);
}
