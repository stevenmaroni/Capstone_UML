
void setup() {
  Serial.begin(9600);

}

void loop() {
 int microphone = analogRead(A1);
  Serial.print("MicrophoneVoltage:");
  Serial.println(microphone);
  //delay(100);

}
