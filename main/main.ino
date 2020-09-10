byte acq_buffer;
byte last_acq_bit = 1;
int start_seq_counter = 0;

long START_SEQUENCE = 0xEB90;
long start_buffer = 0;

int IDLE_SEQUENCE = 0x55;
byte recv_buffer[8];
int recv_bytes = 0;

int state = 0;
int STATE_ACQ_LED = 2;
int STATE_START_LED = 4;
int STATE_RECV_LED = 6;

// If not zero: 
//   - If filler = 1: reject
//   - If filler = 0: check if error is in error table and is not 1:
//       -> flip that bit and report error corrected
//   - Reject

// For transfer layer:
// If not zero: reject

// Output:
// Flag: AD/BC/BD
// Data: 010101010101010101010101010101010101010101010........................

void setup() {
  // put your setup code here, to run once:
  Serial.begin(2400);
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
      // Increment the amount of octets we received already, unless we received an idle sequence
      // instead of a CLTU. So idle sequences are recorded, but later overwritten.
      if (recv_bytes > 0 || recv_buffer[0] != IDLE_SEQUENCE) {
        recv_bytes++;
      }
      if (recv_bytes >= 8) {
        // Reset counter
        recv_bytes = recv_bytes % 8;
        // Process CLTU HERE
        Serial.println("<Received CLTU>");
        for (int i=0; i<8; i++) {
          Serial.print(">");
          Serial.print(i);
          Serial.print(": ");
          printBits(recv_buffer[i]);
          Serial.print("\n");
        }
        byte remainder = crc(recv_buffer);
        Serial.print("Remainder: ");
        Serial.println(remainder, BIN);

        if (remainder == 0) {
          Serial.println("CLTU approved");
        } else {
          boolean correctable = false;
          for (int i=0; i<8; i++) {
            for (int j=0; j<8; j++) {
              if (correctable == false) {
                recv_buffer[i] = recv_buffer[i] ^ (B1 << 7-j);
                if (crc(recv_buffer) == 0) {
                  correctable = true;
                  Serial.print("Correctable error found in position ");
                  Serial.println(i*8 + j);
                } else {
                  recv_buffer[i] = recv_buffer[i] ^ (B1 << 7-j);
                }
              }
            }
          }
          if (remainder == 1) {
            correctable = false;
          }
          if (correctable == false) {
            Serial.println("CLTU rejected");
          } else {
            Serial.println("<Corrected CLTU>");
            for (int i=0; i<8; i++) {
              Serial.print(">");
              Serial.print(i);
              Serial.print(": ");
              printBits(recv_buffer[i]);
              Serial.print("\n");
            }
          }
        } 
      }
    }
  }
}

byte recvBin() {
  byte recv_buffer = Serial.read() - 48;
  return recv_buffer;
}

void printBits(byte myByte){
 for(byte mask = 0x80; mask; mask >>= 1){
   if(mask  & myByte)
       Serial.print('1');
   else
       Serial.print('0');
 }
}

byte crc(byte cltu_blocks[8]) {
  byte bin_array[63];
  byte mask = B10000000;
  byte result = 0;
  for (int i=0; i<8; i++){
    byte block = cltu_blocks[i];
    for (int j = 0; j<8; j++){
      if (i == 7 && j == 7) {
        break;
      } else {
        bin_array[i*8 + j] = (block & (mask >> j)) >> (7-j);
      }
    }
  }

  for (int i=0; i<56; i++) {
    if(bin_array[i] == 1) {
      // XOR 11000101
      // x7 + x6 + x2 + 1
      bin_array[i] = 1 - bin_array[i];
      bin_array[i+1] = 1 - bin_array[i+1];
      bin_array[i+5] = 1 - bin_array[i+5];
      bin_array[i+7] = 1 - bin_array[i+7];
    }
  }

  //{0, 0, 0, 0, 0, 0, 0, ..........., 1, 0, 1, 1, 1, 0, 0, 0} -> 10111000
  for (int i=56; i<63; i++) {
    result = result + (bin_array[i] << (62-i));
  }

  return result;
}
