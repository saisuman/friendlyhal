
void buzzAtFrequency(int freq) {
  pinMode(17, OUTPUT);
  for (int i = 0; i < 1000; ++i ) {
    
digitalWrite(17, HIGH);
delayMicroseconds(1000000 / freq);
  digitalWrite(17, LOW);
  }
  
}

void setup() {
  buzzAtFrequency(27);
  delay(100);
  buzzAtFrequency(30);
  delay(100);
  buzzAtFrequency(32);
  delay(100);
  buzzAtFrequency(36);
  
  pinMode(3, INPUT);
  Serial.begin(9600);
  Serial.println("Initialising.");
  Serial.println("Finished init."); 
}

unsigned long TIMEOUT_COUNTER = 1600;  // 100 usec

unsigned long get_twiddle_length(int pin) {
  unsigned long counter = TIMEOUT_COUNTER;
  while (digitalRead(pin) == LOW && --counter) { }
  if (counter == 0) {
    return 0;
  }
  unsigned long t1 = micros();
  while (digitalRead(pin) == HIGH && --counter) { }
  return counter == 0 ? counter : micros() - t1;
}

boolean wait_for_low(int pin) {
  unsigned long counter = TIMEOUT_COUNTER;
  while (digitalRead(pin) == HIGH && --counter) { }
  return counter != 0;
}

boolean wait_for_high(int pin) {
  unsigned long counter = TIMEOUT_COUNTER;
  while (digitalRead(pin) == LOW && --counter) { }
  return counter != 0;
}

int get_byte(boolean* b, int num) {
  int ret = 0;
  for (int i = 0; i < 8; ++i) {
    ret = ret | ((b[i + num * 8] ? 1 : 0) <<  (7 - i));
  }
  return ret;  
}

void loop() {  
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  delay(4000);
  digitalWrite(2, LOW);
  delay(18);
  digitalWrite(2, HIGH);
  delayMicroseconds(40);
  pinMode(2, INPUT);
  
  // DHT response.
  if (!wait_for_high(2)) { return; }
  if (!wait_for_low(2)) { return; }
  
  boolean bits[40];
  for (int i = 0; i < 40; ++i) {
    unsigned long val = get_twiddle_length(2);
    if (val == 0) {
      Serial.println("Read error.");
      return;  // Handle error.
    }
    bits[i] = val > 40;  // 26-28us is 0, 70us is 1.
  }
  
  int hi = get_byte(bits,0);
  int hd = get_byte(bits, 1);
  int ti = get_byte(bits, 2);
  int td = get_byte(bits, 3);
  int csum = get_byte(bits, 4);
  Serial.print(hi);
  Serial.print(".");
  Serial.print(hd);
  Serial.print(" ");
  Serial.print(ti);
  Serial.print(".");
  Serial.print(td);
  Serial.println(" ");
  
  byte csum_check = hi +  hd + ti + td;
}

