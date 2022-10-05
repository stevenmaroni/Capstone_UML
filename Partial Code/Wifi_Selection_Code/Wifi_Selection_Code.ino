#include <SPI.h>
#include <WiFiNINA.h>
#include <Adafruit_GFX.h>
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
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

void setup() {
  bool WifiCheck = true;
  int page = 0;
  Serial.begin(9600);
  tft.begin();
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
  }

  int numNetworks = WiFi.scanNetworks();
  Serial.println(numNetworks);
  printWifiSelection(page, numNetworks);
  while(WifiCheck){
    TSPoint p = ts.getPoint();
    if (p.z > ts.pressureThreshhold) {
      p.x = map(p.x, TS_MINX, TS_MAXX, tft.height(), 0);
      p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.width());
      //Serial.println("x div: ");
      //Serial.println(p.x);
      //Serial.println("\ny div: ");
      //Serial.println(p.y);
      if(p.y >= 5 && p.x >= 10 && p.y <= 475 && p.x <= 310){
        if( p.y >= 80 && p.y
        <= 400){
          continue;
        }
        else if(p.y < 80 && page > 0){
          page = page -1;
          printWifiSelection(page, numNetworks);
        }
        else if(p.y > 400 && ((page +1) * 5) < numNetworks){
          page = page + 1;
          printWifiSelection(page, numNetworks);
        }
      }
    }
  }
}

void printWifiSelection(int page, int numOfNetworks){
  int index;
  String wifiName;
  tft.fillScreen(HX8357_CYAN);
  tft.setRotation(1);
  tft.setTextSize(2);
  tft.setTextColor(HX8357_WHITE, HX8357_BLACK);
  for(index = 0; (index < 5) && ((index + (page * 5)) < numOfNetworks); index++){
    drawButton(80, 10 + (60 *index), 320, 56);
    tft.setCursor(90, 20 + (60 * index));
    wifiName = WiFi.SSID(index + (page * 5));
    if(wifiName.length() > 20){
      wifiName = wifiName.substring(0,20);
      wifiName.concat("...");
    }
    tft.print(wifiName);
    tft.setCursor(90, 45 + (60 * index));
    tft.print(WiFiStrength(index + (page *5)));
  }
  if(page >0){
    drawButton(5, 10, 70, 300);
    tft.fillTriangle(20, 160, 60, 135, 60, 185,HX8357_WHITE);
  }
  if(((page +1) * 5) < numOfNetworks){
    drawButton(405, 10 , 70, 300);
    tft.fillTriangle(465, 160, 425, 135, 425, 185,HX8357_WHITE);
  }
}

void drawButton(int x, int y, int w, int h)
{
  tft.fillRoundRect(x, y, w, h, 3, HX8357_WHITE);
  tft.fillRoundRect(x + 1, y + 1, w - 1 * 2, h - 1 * 2, 3, HX8357_BLACK);
}

String WiFiStrength(int index){
  String strength = "Strength: ";
  int dBm = WiFi.RSSI(index);
    if(dBm <= -100)
    {
      strength.concat("0");
    }
    else if(dBm >= -50)
    {  
      strength.concat("100");
    }
    else
    {
      strength.concat(String(2 * (dBm + 100)));
    }
    strength.concat("%");
   return strength;
}


void loop() {
  // put your main code here, to run repeatedly:

}
