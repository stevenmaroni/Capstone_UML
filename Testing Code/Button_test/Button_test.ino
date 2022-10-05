
int wifiLED = 5;
int wifiPin = 4;
int testLED = 3;
int testPin = 2;
int WifiVal = 0;
int testVal = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(wifiPin, INPUT);
  pinMode(testVal, INPUT);
  pinMode(wifiLED, OUTPUT);
  pinMode(testLED, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  WifiVal = digitalRead(wifiPin);
  testVal = digitalRead(testPin);
  if (WifiVal == HIGH){
    Serial.println("Wifi High");
  }
  if (testVal == HIGH){
    Serial.println("Test High");
  }
  digitalWrite(wifiLED, WifiVal);
  digitalWrite(testLED, testVal);

}
