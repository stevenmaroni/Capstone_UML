#include <Adafruit_GFX.h>
#include <SPI.h>
#include "Adafruit_HX8357.h"
#include "TouchScreen.h"
#include <avr/pgmspace.h>

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
  Serial.begin(9600);
  tft.begin();
  TSPoint p;

  tft.fillScreen(HX8357_BLACK);

}

void loop() {
  p = ts.getPoint();
  if (p.z > ts.pressureThreshhold) {
    p.x = map(p.x, TS_MINX, TS_MAXX, tft.height(), 0);
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.width());
    Serial.print("x Location:");
    Serial.println(p.x);
    Serial.print("y Location");
    Serial.println(p.y);
  }
  delay(100);
}
