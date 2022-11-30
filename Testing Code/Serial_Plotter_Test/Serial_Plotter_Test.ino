
void setup() {
  Serial.begin(9600);

}

void loop() {
 int microphone = analogRead(A1);
  Serial.print("MicrophoneVoltage:");
  Serial.print(microphone);
  Serial.print(",");
  Serial.print("Gate:");
  Serial.println(analogRead(A4));
  //delay(100);

}
