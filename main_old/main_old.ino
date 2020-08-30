char incoming_value = 0;
int acq_seq_count = 0;
int LED_PIN_ACQ = 11;
int LED_PIN_START = 12;
int LED_PIN_CONN = 13;
int state = 0;
char start_seq_buffer[16] = {""};
char start_seq[16] = {0b1, 0b1, 0b1, 0b0, 0b1, 0b0, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0};
int start_seq_error = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN_ACQ, OUTPUT);
  pinMode(LED_PIN_START, OUTPUT);
  pinMode(LED_PIN_CONN, OUTPUT);
  digitalWrite(LED_PIN_ACQ, HIGH);
  digitalWrite(LED_PIN_START, LOW);
  digitalWrite(LED_PIN_CONN, LOW);
}

void loop() {
  if (state == 0){
    //Looking for acquisition sequence
    if(Serial.available() > 0) {
      incoming_value = Serial.read();
      // Read back incoming value.
      Serial.print(incoming_value);
      Serial.print("\n");
      // TO DO: Make it count alternating 1's and 0's.
      acq_seq_count++;
    }
    if (acq_seq_count == 64) {
      // Go to next state, search for start sequence
      Serial.print("Acquisition sequence passed, state 1\n");
      state = 1;
      digitalWrite(LED_PIN_ACQ, LOW);
      digitalWrite(LED_PIN_START, HIGH);
    }
  }

  if (state == 1){
    // Looking for EB90 sequence
    // Reset the variable used to count the start sequence data buffer
    start_seq_error = 0;
    if (Serial.available() > 0) {
      for (int i=0; i<16; i++){
        // Shift all entries in the buffer one place to the left
        // TO DO: Implement circular buffer? This is very inefficient
        start_seq_buffer[i] = start_seq_buffer[i+1];
      }
      // Read ASCII data ("0" = 48, "1" = 49) into buffer as binary 0 and 1.
      start_seq_buffer[15] = Serial.read() - 48;
      for (int i=0; i<16; i++) {
        // Print back content of buffer for debugging purposes
        Serial.print(start_seq_buffer[i], BIN);
      }
      Serial.print("\n");
      for (int i=0; i<16; i++) {
        // Count entries and compare to reference sequence.
        if (start_seq_buffer[i] != start_seq[i]) {
          start_seq_error++;
        }
      }
      if (start_seq_error == 1 | start_seq_error == 0) {
        // If 0 or 1 error, go to state 2
        state = 2;
        Serial.print("Start sequence passed, state 2\n");
        digitalWrite(LED_PIN_START, LOW);
        digitalWrite(LED_PIN_CONN, HIGH);
      }
    }
  }
}
