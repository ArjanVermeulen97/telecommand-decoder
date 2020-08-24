char incoming_value = 0;
int init_seq_count = 0;
int LED_PIN = 13;
int state = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  if (state == 0){
    if(Serial.available() > 0) {
      incoming_value = Serial.read();
      Serial.print(incoming_value);
      Serial.print("\n");
      init_seq_count++;
    }
    if (init_seq_count == 64) {
      Serial.print("Start sequence passed, state 1\n");
      state = 1;
      digitalWrite(LED_PIN, HIGH);
    }
  }
}
