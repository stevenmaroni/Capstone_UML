int LED = 5;
int test = 2;

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(test, INPUT);
}

void loop() {
  unsigned long Timesincefirst;
  int runs = 0;
  bool check;
  if(analogRead(A1) >= 120){
    check = true;
    Timesincefirst = millis();
    while(runs < 5 && check){
      delay(100);
      if(analogRead(A1) >= 120){
        runs = runs + 1;
      }
      else{
        check = false;
      }
    }
    if(check){
      delay(250);
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
        digitalWrite(LED, HIGH);
      }
    }
  }
}
