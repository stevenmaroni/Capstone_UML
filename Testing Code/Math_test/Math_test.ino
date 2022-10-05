void setup(){
Serial.begin(9600);
  for(int x = 0; x < 320; x++){
    Serial.print(x);
    Serial.print(floor((x - 70)/50));
    Serial.print('\n');
  }
};

void loop(){

};
