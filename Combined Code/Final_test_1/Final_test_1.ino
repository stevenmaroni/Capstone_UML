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
bool Upper;
bool Special;

Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

/////////////////////////
// PIN and other varaiables 
/////////////////////////

int wifiLED = 5;
int wifiPin = 4;
int testLED = 12;
int testPin = 11;
int LEDPin = 6;
int ShakerPin = 3;
int WifiVal = 0;
int testVal = 0;
bool PressedWiFi = false;
bool PressedTest = false;
unsigned long TimePassedWiFi;
unsigned long TimePassedTest;

int stat = WL_IDLE_STATUS;
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

int yLocation(int y)
{
  return floor((y - 70)/50);
}

int xLocation(int x)
{
  return floor((x - 30)/42);
}

char* Get_wifi_password(){
  bool Upper = true;
  bool Special = false;
  int y_div = 0;
  int x_div = 0;
  static char password[30] = "";
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
        Serial.println("x div: ");
        Serial.println(x_div);
        Serial.println("\ny div: ");
        Serial.println(y_div);
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
          Serial.println("Ran space");
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
        if( p.y >= 80 && p.y <= 400){
          if( p.x >= 10 && p.x < 70){
            if(WiFiConfirm(0)){
              return ((page *5));
            }
          }
          else if( p.x >= 70 && p.x < 130){
            if(WiFiConfirm(1)){
              return ((page *5)+1);
            }
          }
          else if( p.x >= 130 && p.x < 190){
            if(WiFiConfirm(2)){
              return ((page *5)+2);
            }
          }
          else if( p.x >= 190 && p.x < 250){
            if(WiFiConfirm(3)){
              return ((page *5)+3);
            }
          }
          else if( p.x >= 250 && p.x <= 310){
            if(WiFiConfirm(4)){
              return ((page *5)+4);
            }
          }
          printWifiSelection(page, numNetworks);
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

bool WiFiConfirm(int index){
  tft.fillRoundRect(300, 15 + (60*index) , 100, 46,3,HX8357_WHITE);
  tft.fillRoundRect(300, 16 + (60*index), 98, 44, 3, HX8357_GREEN);
  tft.setTextColor(HX8357_BLACK, HX8357_GREEN);
  tft.setCursor(310, 30 + (60 *index));
  tft.print("Confirm");
  tft.setTextColor(HX8357_WHITE, HX8357_BLACK);
  delay(100);
  while(true){
    TSPoint p = ts.getPoint();
    if (p.z > ts.pressureThreshhold) {
      p.x = map(p.x, TS_MINX, TS_MAXX, tft.height(), 0);
      p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.width());
      if(p.y >= 5 && p.x >= 10 && p.y <= 475 && p.x <= 310){
        if(p.y >= 300 && p.y <= 400 && (p.x >= (15 + (60 *index))) && (p.x <= (61 + (60* index)))){
          return true;
        }
        else{
          return false;
        }
      }
    }
  }
}

/////////////////////////////
// Common Functions
/////////////////////////////

void drawButton(int x, int y, int w, int h)
{
  tft.fillRoundRect(x, y, w, h, 3, HX8357_WHITE);
  tft.fillRoundRect(x + 1, y + 1, w - 1 * 2, h - 1 * 2, 3, HX8357_BLACK);
}

/////////////////////////////
// Start of Main
/////////////////////////////
void setup() {
  int WifiIndex;
  char* password = "";
  Serial.begin(9600);
  tft.begin();
  while (!Serial) {
    continue;
  }
  WifiIndex = Wifi_Selection();
  if(WiFi.encryptionType(WifiIndex) != ENC_TYPE_NONE){
    password = Get_wifi_password();
  }
  tft.fillScreen(HX8357_CYAN);
  tft.setTextColor(HX8357_BLACK, HX8357_CYAN);
  if(WiFi.encryptionType(WifiIndex) == ENC_TYPE_NONE){
    while( stat != WL_CONNECTED){
      tft.print("Connecting to WiFi");
      stat = WiFi.begin(WiFi.SSID(WifiIndex));
      delay(5000);
    }
  }
  else{
    while( stat != WL_CONNECTED){
      tft.print("Connecting to WiFi");
      stat = WiFi.begin(WiFi.SSID(WifiIndex), password);
      delay(5000);
    }
  }
  tft.fillScreen(HX8357_CYAN);
  pinMode(wifiPin, INPUT);
  pinMode(testVal, INPUT);
  pinMode(wifiLED, OUTPUT);
  pinMode(testLED, OUTPUT);
  pinMode(LEDPin, OUTPUT);
  pinMode(ShakerPin, OUTPUT);
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
    digitalWrite(LEDPin, HIGH);
    digitalWrite(ShakerPin, HIGH);
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
    digitalWrite(LEDPin, LOW);
    digitalWrite(ShakerPin, LOW);
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
    PressedTest = false;
    PressedWiFi = false;
    setup();
  }
}
