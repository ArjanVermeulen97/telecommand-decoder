int RX_PIN = 2;
int TX_PIN = 4;
int CLOCK_PIN = 6;
int state = 0;
int clockState = 0;
unsigned long clockStart;
int clockCounter = 0;
int clockInterval;
// Overview of states in Latex

void setup() {
  pinMode(RX_PIN, INPUT_PULLUP);
  pinMode(TX_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
}

void loop() {
  if (state == 0) {
    // Acquire clock signal

    // Debounce
    //delay(1);
    
    if (clockCounter == 0) {
      if (digitalRead(RX_PIN) == HIGH) {
        clockCounter++;
        clockStart = millis();
        clockState = 1;
      } 
  
    if (clockState == 1) {
      if (digitalRead(RX_PIN) == LOW) {
        clockCounter++;
        clockState = 0;
      }
    } else {
      if (digitalRead(RX_PIN) == HIGH) {
        clockCounter++;
        clockState = 1;
      }
    }
    
    }

    if (clockCounter == 64) {
      clockInterval = round((millis() - clockStart)/64);
      state = 1;
    }
    
  }

  if (state == 1) {
    digitalWrite(CLOCK_PIN, HIGH);
    delay(clockInterval);
    digitalWrite(CLOCK_PIN, LOW);
    delay(clockInterval);
  }

}
