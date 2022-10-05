#include <WiFiNINA.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.scanNetworks();
  String test = WiFi.SSID(0);
  String test2 = test.substring(0,30);
  Serial.println(test);
  Serial.println(test2);
  Serial.println(WiFi.SSID(0));
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
