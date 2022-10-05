#include <Adafruit_GFX.h>
#include <SPI.h>
#include "Adafruit_HX8357.h"
#include "TouchScreen.h"
#include <avr/pgmspace.h>
#include <WiFiNINA.h>
#include "AnotherIFTTTWebhook.h"


///////////////////////////
// Touch Screen Variables
//////////////////////////
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
bool Upper = true;
bool Special = false;

Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

/////////////////////////
// Other Variables
/////////////////////////

int wifiLED = 5;
int wifiPin = 4;
int testLED = 3;
int testPin = 2;
int WifiVal = 0;
int testVal = 0;

////////////////////////////////
// Keyboard Setup and Functions
////////////////////////////////

const char Keypad_basic_lower[4][10] PROGMEM = {
  {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'},
  {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'},
  {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'z'},
  {' ', ' ', 'x', 'c', 'v', 'b', 'n', 'm', ' ', ' '},
};

const char Keypad_basic_upper[4][10] PROGMEM = {
  {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'},
  {'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P'},
  {'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'Z'},
  {' ', ' ', 'X', 'C', 'V', 'B', 'N', 'M', ' ', ' '},
};

const char Keypad_Special[4][10] PROGMEM = {
  {'[', ']', '{', '}', '#', '%', '^', '*', '+', '='},
  {'-', '/', ':', ';', '(', ')', '$', '&', '@', '"'},
  {'.', '\,', '?', '!', '\'','.', '\,', '?', '!', '\\'},
  {' ', ' ', '\'','_', '|', '~', '<', '>', ' ', ' '}
  
};

void MakeKB_Button(const char type[][10])
{
  tft.setTextSize(2);
  tft.setTextColor(HX8357_WHITE, HX8357_BLACK);
  for (int y = 0; y < 3; y++)
  {
    for (int x = 0; x < 10; x++)
    {
      drawButton(30 + (42 * x),70 + (50 * y), 40, 45);
      tft.setCursor(40 + (42 * x), 85 + (50 * y));
      tft.print(char(pgm_read_byte(&(type[y][x]))));
    }
  }
  for (int x = 2; x < 8; x++)
  {
    drawButton(114 + (42 * (x -2)),220, 40, 45);
    tft.setCursor(124 + (42 * (x -2)), 235);
    tft.print(char(pgm_read_byte(&(type[3][x]))));
  }
  //ShiftKey
  drawButton(30, 220, 82, 45);
  tft.setCursor(35, 235);
  if(Upper){
    tft.print(F("lower"));
  }
  else{
    tft.print(F("UPPER"));
  }

  //Special Characters
  drawButton(30, 270, 82, 45);
  tft.setCursor(35, 290);
  if(Special){
    tft.print(F("Alpha"));
  }
  else{
    tft.print(F("Symbol"));
  }

  //BackSpace
  drawButton(366, 220, 82, 45);
  tft.setCursor(371, 235);
  tft.print(F("Back"));

  //Return
  drawButton(366, 270, 82, 45);
  tft.setCursor(371, 290);
  tft.print(F("Enter"));

  //Spacebar
  drawButton(114, 270, 252, 45);
  tft.setCursor(170, 290);
  tft.print(F("SPACE BAR"));
}

void drawButton(int x, int y, int w, int h)
{
  tft.fillRoundRect(x, y, w, h, 3, HX8357_WHITE);
  tft.fillRoundRect(x + 1, y + 1, w - 1 * 2, h - 1 * 2, 3, HX8357_BLACK);
}

int yLocation(int y)
{
  return floor((y - 70)/50);
}

int xLocation(int x)
{
  return floor((x - 30)/42);
}

char Get_wifi_password(){
  int y_div = 0;
  int x_div = 0;
  char password[30] = "";
  int indexBuf = 0;
  tft.fillScreen(HX8357_CYAN);
  tft.setRotation(1);
  MakeKB_Button(Keypad_basic_upper);
  tft.fillRect(29, 19, 422, 42, HX8357_BLACK);
  tft.fillRect(30, 20, 420, 40, HX8357_WHITE);
  bool EnterCheck = true;

  // Keyboard interactions
  
  while(EnterCheck){
    TSPoint p = ts.getPoint();
    if (p.z > 100) {
      p.x = map(p.x, TS_MINX, TS_MAXX, tft.height(), 0);
      p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.width());
      if(p.x >= 70 && p.y >= 30){
        y_div = yLocation(p.x);
        x_div = xLocation(p.y);
        if((y_div < 3 && x_div >= 0 && x_div < 10) || (y_div == 3 && x_div > 1 && x_div < 8)){
          if(Special){
            password[indexBuf] = pgm_read_byte(&(Keypad_Special[y_div][x_div]));
            indexBuf += 1;
          }
          else{
            if(Upper){
              password[indexBuf] = pgm_read_byte(&(Keypad_basic_upper[y_div][x_div]));
              indexBuf += 1;
            }
            else{
              password[indexBuf] = pgm_read_byte(&(Keypad_basic_lower[y_div][x_div]));
              indexBuf += 1;
            }
          }
          tft.fillRect(30, 20, 420, 40, HX8357_WHITE);
          tft.setTextSize(2);
          tft.setTextColor(HX8357_BLACK, HX8357_WHITE);
          tft.setCursor(35, 35);
          tft.println(password);
        }
        else if( y_div ==3 && (x_div == 0 || x_div == 1)){
          if(!Special){
            if(Upper){
              Upper = false;
              MakeKB_Button(Keypad_basic_lower);
            }
            else{
              Upper = true;
              MakeKB_Button(Keypad_basic_upper);
            }
          }
          else{
            continue;
          }
        }
        else if( y_div ==4 && (x_div == 0 || x_div == 1)){
          if(Special){
            Special = false;
            if(Upper){
              MakeKB_Button(Keypad_basic_upper);
            }
            else{
              MakeKB_Button(Keypad_basic_lower);
            }
          }
          else{
            Special = true;
            MakeKB_Button(Keypad_Special);
          }
        }
        else if(y_div == 3 && (x_div == 8 || x_div == 9)){
          password[indexBuf - 1] = NULL
          ;
          indexBuf -= 1;

          tft.fillRect(30, 20, 420, 40, HX8357_WHITE);
          tft.setTextSize(2);
          tft.setTextColor(HX8357_BLACK, HX8357_WHITE);
          tft.setCursor(35, 35);
          tft.println(F(password));
        }
        else if(y_div == 4 && (x_div == 8 || x_div == 9)){
          EnterCheck = false;
        }
        else if(y_div == 4 && (x_div > 1 && x_div < 8)){
          password[indexBuf] = ' ';
          indexBuf += 1;
          delay(100);
        }
        else{
          continue;
        }
      }
    }
    
  }
  return password;
}

//////////////////////////////
// Wifi Selection Function
///////////////////////////////
int Wifi_Selection(){
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
  }

  int numSsid = WiFi.scanNetworks();
  printWifiSelection()
  
}

void printWifiSelection(){
  
}

/////////////////////////////
// Start of Main
/////////////////////////////
void setup() {
  char password[30];
  Serial.begin(9600);
  tft.begin();
  while (!Serial) {
    continue;
  }
  password = Get_wifi_password();
}

void loop() {
  // put your main code here, to run repeatedly:

}