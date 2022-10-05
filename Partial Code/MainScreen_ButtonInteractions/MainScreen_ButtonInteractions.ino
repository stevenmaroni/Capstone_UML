#include <Adafruit_GFX.h>
#include <SPI.h>
#include "Adafruit_HX8357.h"
#include "TouchScreen.h"

#define YP A2
#define XM A3
#define YM 7
#define XP 8
#define TFT_RST -1
#define TFT_DC 9
#define TFT_CS 10

#define TS_MINX 110
#define TS_MINY 80
#define TS_MAXX 900
#define TS_MAXY 940

Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);
#include <avr/pgmspace.h>

int wifiLED = 5;
int wifiPin = 4;
int testLED = 3;
int testPin = 2;
int WifiVal = 0;
int testVal = 0;
bool PressedWiFi = false;
bool PressedTest = false;
unsigned long TimePassedWiFi;
unsigned long TimePassedTest;

void setup() {
  pinMode(wifiPin, INPUT);
  pinMode(testVal, INPUT);
  pinMode(wifiLED, OUTPUT);
  pinMode(testLED, OUTPUT);
  
  Serial.begin(9600);
  tft.begin();
  tft.fillScreen(HX8357_CYAN);
  tft.setRotation(1);
  tft.setTextSize(4);
  tft.setTextColor(HX8357_BLACK, HX8357_CYAN);
  tft.setCursor(50, 100);
  tft.println(F("Demo of Buttons"));
}

void loop() {
  WifiVal = digitalRead(wifiPin);
  testVal = digitalRead(testPin);
  digitalWrite(wifiLED, WifiVal);
  digitalWrite(testLED, testVal);
  
  //Wifi Button
  if(!PressedWiFi && WifiVal == HIGH){
    PressedWiFi = true;
    TimePassedWiFi = millis();
  }
  else if(PressedWiFi && WifiVal == HIGH){
  }
  else if(PressedWiFi && WifiVal == LOW){
    PressedWiFi = false;
  }

  //Test Button
  if(!PressedTest && testVal == HIGH){
    PressedTest = true;
    TimePassedTest = millis();
  }
  else if(PressedTest && testVal == HIGH){
  }
  else if(PressedTest && testVal == LOW){
    PressedTest = false;
  }

  //Test Check
  if(PressedTest && (millis() - TimePassedTest) > 5000){
    digitalWrite(wifiLED, LOW);
    digitalWrite(testLED, LOW);
    tft.fillScreen(HX8357_RED);
    tft.setTextColor(HX8357_WHITE, HX8357_RED);
    tft.println(F("Test Activated!\nPress Test Button for 5 secs to return"));
    PressedTest = false;
    while(!PressedTest || (millis() - TimePassedTest) <= 5000){
      testVal = digitalRead(testPin);
      digitalWrite(testLED, testVal);
      if(!PressedTest && testVal == HIGH){
        PressedTest = true;
        TimePassedTest = millis();
      }
      else if(PressedTest && testVal == HIGH){
      }
      else if(PressedTest && testVal == LOW){
        PressedTest = false;
      }
    }
    PressedTest = false;
    PressedWiFi = false;
    tft.fillScreen(HX8357_CYAN);
    tft.setTextSize(4);
    tft.setTextColor(HX8357_BLACK, HX8357_CYAN);
    tft.setCursor(50, 100);
    tft.println(F("Demo of Buttons"));
  }

  //Wifi Check
  if(PressedWiFi && (millis() - TimePassedWiFi) > 5000){
    digitalWrite(wifiLED, LOW);
    digitalWrite(testLED, LOW);
    tft.fillScreen(HX8357_BLACK);
    tft.setTextColor(HX8357_WHITE, HX8357_BLACK);
    tft.println(F("WIFI Activated!\nPress Test Button to return"));
    while(testVal != HIGH){
        testVal = digitalRead(testPin);
        digitalWrite(testLED, testVal);
      }
    PressedTest = false;
    PressedWiFi = false;
    tft.fillScreen(HX8357_CYAN);
    tft.setTextSize(4);
    tft.setTextColor(HX8357_BLACK, HX8357_CYAN);
    tft.setCursor(50, 100);
    tft.println(F("Demo of Buttons"));
  }
  
  
}
