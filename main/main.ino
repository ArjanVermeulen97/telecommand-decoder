byte acq_buffer;
byte last_acq_bit = 1;
int start_seq_counter = 0;

long START_SEQUENCE = 0xEB90;
long start_buffer = 0;

byte recv_buffer[8];
int recv_bytes = 0;

int state = 0;
int STATE_ACQ_LED = 2;
int STATE_START_LED = 4;
int STATE_RECV_LED = 6;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(STATE_ACQ_LED, OUTPUT);
  pinMode(STATE_START_LED, OUTPUT);
  pinMode(STATE_RECV_LED, OUTPUT);
  digitalWrite(STATE_ACQ_LED, HIGH);
  digitalWrite(STATE_START_LED, LOW);
  digitalWrite(STATE_RECV_LED, LOW);
  Serial.println("<Receiver is ready>");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (state == 0) {
    if (Serial.available() > 0) {
      acq_buffer = recvBin();
      if (acq_buffer ^ last_acq_bit == B1){
        start_seq_counter += 1;
        last_acq_bit = acq_buffer;
      }
      if (last_acq_bit == 1 && start_seq_counter >= 64) {
        Serial.println("<Acquisition sequence detected>");
        state = 1;
        digitalWrite(STATE_START_LED, HIGH);
        digitalWrite(STATE_ACQ_LED, LOW);
      }
    }
  } else if (state == 1) {
    // Receive new bit into start_buffer
    if (Serial.available() > 0) {
      start_buffer = start_buffer << 1;
      start_buffer = start_buffer + recvBin();
      start_buffer = start_buffer % 65536;
      if (start_buffer == START_SEQUENCE) {
        Serial.println("<Start sequence detected>");
        state = 2;
        digitalWrite(STATE_RECV_LED, HIGH);
        digitalWrite(STATE_START_LED, LOW);
      } else {
        // Check if there's a single error, then approve anyway
        for (int i=0; i<16; i++) {
          long error_start_buffer = start_buffer ^ (B1 << i);
          if (error_start_buffer == START_SEQUENCE) {
            Serial.println("<Start sequence detected>");
            Serial.println("-- One error detected and corrected in start sequence --");
            state = 2;
            digitalWrite(STATE_RECV_LED, HIGH);
            digitalWrite(STATE_START_LED, LOW);
          }
        }
      }
    }
  } else if (state == 2) {
    // Receive octets
    if (Serial.available() >= 8) {
      for (int i=0; i<8; i++) {
        if (i == 0){
          recv_buffer[recv_bytes] = 0;
        }
        recv_buffer[recv_bytes] = recv_buffer[recv_bytes] + (recvBin() << (7 - i));
      }
      // Increment the amount of octets we received already
      recv_bytes++;
      if (recv_bytes >= 8) {
        // Reset counter
        recv_bytes = recv_bytes % 8;
        // Process CLTU HERE
        Serial.println("<Received CLTU>");
        for (int i=0; i<8; i++) {
          Serial.println(recv_buffer[i], BIN);
        }
      }
    }
  }
}

byte recvBin() {
  byte recv_buffer = Serial.read() - 48;
  return recv_buffer;
}
