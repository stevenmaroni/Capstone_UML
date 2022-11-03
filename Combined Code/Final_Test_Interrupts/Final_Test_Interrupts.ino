#include <Adafruit_GFX.h>
#include <SPI.h>
#include "Adafruit_HX8357.h"
#include "TouchScreen.h"
#include <avr/pgmspace.h>
#include <WiFiNINA.h>


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
WiFiClient client;

Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

/////////////////////////
// PIN and other varaiables 
/////////////////////////
int wifiPin = 4;
int testLED = 5;
int testPin = 11;
int LEDPin = 6;
int ShakerPin = 3;
int LEDIndicator = 2;
unsigned long TimePassedAlarm;
bool Aware;
bool ActiveFire = false;
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
  Upper = true;
  Special = false;
  int y_div = 0;
  int x_div = 0;
  static char password[30];
  int indexBuf = 0;
  for(int l; l < 30; l++){
    password[l] = '\0';
  }
  tft.fillScreen(HX8357_CYAN);
  tft.setRotation(1);
  MakeKB_Button(Keypad_basic_upper);
  tft.fillRect(29, 19, 422, 42, HX8357_BLACK);
  tft.fillRect(30, 20, 420, 40, HX8357_WHITE);
  bool EnterCheck = true;

  // Keyboard interactions
  Upper = true;
  Special = false;
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
          else if( p.x >= 70 && p.x < 130 ){
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
            if((((page*5) +3) < numNetworks)){
              if(WiFiConfirm(3)){
                return ((page *5)+3);
              }
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

void Wifi_Setup(void){
  int stat = WL_IDLE_STATUS;
  int WifiIndex = 0;
  char* password = "";
  WifiIndex = Wifi_Selection();
  if(WiFi.encryptionType(WifiIndex) != ENC_TYPE_NONE){
    password = Get_wifi_password();
  }
  tft.fillScreen(HX8357_CYAN);
  tft.setTextColor(HX8357_BLACK, HX8357_CYAN);
  tft.setTextSize(4);
  tft.setCursor(25, 100);
  tft.println("Connecting Wifi...");
  if(WiFi.encryptionType(WifiIndex) == ENC_TYPE_NONE){
    stat = WiFi.begin(WiFi.SSID(WifiIndex));
    delay(5000);
    if( WiFi.status() != WL_CONNECTED){
      delay(5000);
      if( WiFi.status() != WL_CONNECTED){
        delay(5000);
      }
    }
  }
  else{
    stat = WiFi.begin(WiFi.SSID(WifiIndex), password);
    delay(5000);
    if( WiFi.status() != WL_CONNECTED){
      delay(5000);
      if( WiFi.status() != WL_CONNECTED){
        delay(5000);
      }
    }
  }

  if( stat != WL_CONNECTED){
    MainMenu(0);
  }
  else{
    MainMenu(1);
  }
}

void drawButton(int x, int y, int w, int h)
{
  tft.fillRoundRect(x, y, w, h, 3, HX8357_WHITE);
  tft.fillRoundRect(x + 1, y + 1, w - 1 * 2, h - 1 * 2, 3, HX8357_BLACK);
}

void MainMenu(int type){
  tft.fillScreen(HX8357_CYAN);
  tft.setTextSize(2);
  tft.setTextColor(HX8357_BLACK, HX8357_CYAN);
  if(type == 0){
    tft.fillTriangle(140, 200, 240, 80, 340, 200,HX8357_WHITE);
    tft.setCursor(150, 210);
    tft.print("Active, No Wifi");
  }
  if (type == 1){
    tft.fillTriangle(140, 200, 240, 80, 340, 200,HX8357_GREEN);
    tft.setCursor(150, 210);
    tft.print("Active With Wifi");
  }
  if (type == 2){
    tft.fillTriangle(140, 200, 240, 80, 340, 200,HX8357_YELLOW);
    tft.setCursor(150, 210);
    tft.println("Test Mode Active");
    tft.println("Hold Test/ Silence button for 5 Secs to end test");
    
  }
  if (type == 3){
    tft.setTextColor(HX8357_BLACK, HX8357_RED);
    tft.fillScreen(HX8357_RED);
    tft.fillTriangle(140, 200, 240, 80, 340, 200,HX8357_YELLOW);
    tft.setCursor(150, 220);
    tft.println("FIRE DETECTED");
    tft.println("Press Test/ Silence button to show you are aware");
    tft.setTextColor(HX8357_BLACK, HX8357_CYAN);
  }
}

void send_webhook(String IFTTT_Event, String IFTTT_Key, String IFTTT_Value1, String IFTTT_Value2, String IFTTT_Value3){
  // construct the JSON payload
  String jsonString = "";
  String postString = "";
  String lenString;
  int jsonLength;
  jsonString += "{\"value1\":\"";
  jsonString += IFTTT_Value1;
  jsonString += "\",\"value2\":\"";
  jsonString += IFTTT_Value2;
  jsonString += "\",\"value3\":\"";
  jsonString += IFTTT_Value3;
  jsonString += "\"}";
  jsonLength = jsonString.length();  
  lenString = String(jsonLength);
  postString += "POST /trigger/";
  postString += IFTTT_Event;
  postString += "/with/key/";
  postString += IFTTT_Key;
  postString += " HTTP/1.1\r\n";
  postString += "Host: maker.ifttt.com\r\n";
  postString += "Content-Type: application/json\r\n";
  postString += "Content-Length: ";
  postString += lenString + "\r\n";
  postString += "\r\n";
  postString += jsonString; // combine post request and JSON
  
  client.connect("maker.ifttt.com", 80);
  client.print(postString);
  delay(500);
  client.stop();
}

void TestMode(){
  unsigned long TimePassedTest = millis();
  int testVal = HIGH;
  bool RunTest = false;
  bool PressedTest = false;
  if(ActiveFire){
    Aware = true;
    return;
  }
  digitalWrite(testLED, HIGH);
  while(testVal == HIGH){
    testVal = digitalRead(testPin);
    if((millis() - TimePassedTest) > 5000){
      RunTest = true;
      break;
    }
  }
  digitalWrite(testLED, LOW);
  if(RunTest){
    
    digitalWrite(LEDPin, HIGH);
    digitalWrite(ShakerPin, HIGH);
    MainMenu(2);
    PressedTest = false;
    while(!PressedTest || (millis() - TimePassedTest) <= 5000){
      testVal = digitalRead(testPin);
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
    if(WiFi.status() == WL_CONNECTED){
      MainMenu(1);
    }
    else{
      MainMenu(0);
    }
  }
}

void WiFiMode(){
  digitalWrite(testLED, HIGH);
  int WifiVal = HIGH;
  unsigned long TimePassedWiFi;
  bool RunTest = false;
  TimePassedWiFi = millis();
  while(WifiVal == HIGH){
    WifiVal = digitalRead(wifiPin);
    if((millis() - TimePassedWiFi) > 5000){
      RunTest = true;
      break;
    }
  }
  digitalWrite(testLED, LOW);
  if(RunTest){
    tft.fillScreen(HX8357_CYAN);
    tft.setTextSize(4);
    tft.setTextColor(HX8357_BLACK, HX8357_CYAN);
    tft.setCursor(50, 100);
    tft.println(F("Loading..."));
    Wifi_Setup();
  }
}
/////////////////////////////
// Start of Main
/////////////////////////////
void setup() {
  pinMode(testLED, OUTPUT);
  pinMode(LEDIndicator, OUTPUT);
  digitalWrite(LEDIndicator, HIGH);
  pinMode(wifiPin, INPUT_PULLUP);
  pinMode(testPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(testPin), TestMode, RISING);
  attachInterrupt(digitalPinToInterrupt(wifiPin), WiFiMode, RISING);
  pinMode(LEDPin, OUTPUT);
  pinMode(ShakerPin, OUTPUT);
  Serial.begin(9600);
  tft.begin();
  Wifi_Setup();
  /*
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
  tft.setTextSize(4);
  tft.setTextColor(HX8357_BLACK, HX8357_CYAN);
  tft.setCursor(50, 100);
  tft.println(F("Demo of Buttons"));
  */
}

void loop() {
  int runs = 0;
  bool check;
  
  if(analogRead(A1) >= 120){
    check = true;
    while(runs < 4 && check){
      delay(100);
      if(analogRead(A1) >= 120){
        runs = runs + 1;
        break;
      }
      else{
        check = false;
      }
    }
    if(check){
      delay(350);
      if(analogRead(A1) >= 120){
        check = false;
      }
      delay(100);
      if(analogRead(A1) >= 120){
        check = false;
      }
      delay(350);
      if(analogRead(A1) <= 120){
        check = false;
      }
      delay(100);
      if(analogRead(A1) <= 120){
        check = false;
      }
      if(check){
        digitalWrite(LEDPin, HIGH);
        digitalWrite(ShakerPin, HIGH);
        TimePassedAlarm = millis();
        send_webhook("Fire_Alarm", "9ELX13sgd-VdwIj4UlUoW", "","","");
        MainMenu(3);
        Aware = false;
        ActiveFire = true;
        while(Aware == false){
          if((millis() - TimePassedAlarm) > 5000){
            digitalWrite(LEDPin, LOW);
            digitalWrite(ShakerPin, LOW);
          }

          if((millis() - TimePassedAlarm) > 5000){
            digitalWrite(LEDPin, HIGH);
            digitalWrite(ShakerPin, HIGH);
            TimePassedAlarm = millis();
          }
        }
        digitalWrite(LEDPin, LOW);
        digitalWrite(ShakerPin, LOW);
        if(WiFi.status() == WL_CONNECTED){
          MainMenu(1);
        }
        else{
          MainMenu(0);
        }
        ActiveFire = false;
      }
    }
  }
  /*
  WifiVal = digitalRead(wifiPin);
  testVal = digitalRead(testPin);
  
  //Wifi Button
  if(!PressedWiFi && WifiVal == LOW){
  }
  else if(!PressedWiFi && WifiVal == HIGH){
    PressedWiFi = true;
    TimePassedWiFi = millis();
  }
  else if(PressedWiFi && WifiVal == LOW){
    PressedWiFi = false;
  }
  //Wifi Check
  else if(PressedWiFi && (millis() - TimePassedWiFi) > 5000){d
    tft.fillScreen(HX8357_CYAN);
    tft.setTextSize(4);
    tft.setTextColor(HX8357_BLACK, HX8357_CYAN);
    tft.setCursor(50, 100);
    tft.println(F("Loading..."));
    PressedTest = false;
    PressedWiFi = false;
    Wifi_Setup();
  }
  //Test Button
  if(!PressedTest && testVal == LOW){
  }
  if(!PressedTest && testVal == HIGH){
    PressedTest = true;
    TimePassedTest = millis();
  }
  else if(PressedTest && testVal == LOW){
    PressedTest = false;
  }
  //Test Check
  else if(PressedTest && ((millis() - TimePassedTest) > 5000)){
    digitalWrite(LEDPin, HIGH);
    digitalWrite(ShakerPin, HIGH);
    MainMenu(2);
    PressedTest = false;
    while(!PressedTest || (millis() - TimePassedTest) <= 5000){
      testVal = digitalRead(testPin);
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
    if(WiFi.status() == WL_CONNECTED){
      MainMenu(1);
    }
    else{
      MainMenu(0);
    }
  }
  */
}
