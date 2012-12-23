#include "Arduino.h"
#include "TempHumSensor.h"

// For a 16 MHz microcontroller that's 100 usec.
#define TIMEOUT_COUNTER 1600

static unsigned long getTwiddleLength(int pin) {
  unsigned long counter = TIMEOUT_COUNTER;
  while (digitalRead(pin) == LOW && --counter) { }
  if (counter == 0) {
    return 0;
  }
  unsigned long t1 = micros();
  while (digitalRead(pin) == HIGH && --counter) { }
  return counter == 0 ? counter : micros() - t1;
}

static boolean waitForLow(int pin) {
  unsigned long counter = TIMEOUT_COUNTER;
  while (digitalRead(pin) == HIGH && --counter) { }
  return counter != 0;
}

static boolean waitForHigh(int pin) {
  unsigned long counter = TIMEOUT_COUNTER;
  while (digitalRead(pin) == LOW && --counter) { }
  return counter != 0;
}

static int getByte(boolean* b, int num) {
  int ret = 0;
  for (int i = 0; i < 8; ++i) {
    ret = ret | ((b[i + num * 8] ? 1 : 0) <<  (7 - i));
  }
  return ret;  
}

TempHumSensor::TempHumSensor() {}

void TempHumSensor::init(int pin) {
  pin_ = pin;
  pinMode(pin_, INPUT);
}

bool TempHumSensor::refresh() {
  // Set the value of the temperature to
  // be something that can't possibly be right.
  curr_temp_ = -273 * 100;
  curr_hum_ = -273 * 100;
  pinMode(pin_, OUTPUT);
  digitalWrite(pin_, HIGH);
  delay(200);
  digitalWrite(pin_, LOW);
  delay(18);
  digitalWrite(pin_, HIGH);
  delayMicroseconds(40);
  pinMode(pin_, INPUT);
  
  // DHT response.
  if (!waitForHigh(pin_)) { return false; }
  if (!waitForLow(pin_)) { return false; }
  
  boolean bits[40];
  for (int i = 0; i < 40; ++i) {
    unsigned long val = getTwiddleLength(9);
    if (val == 0) {
      return false;  // Handle error.
    }
    bits[i] = val > 40;  // 26-28us is 0, 70us is 1.
  }
  
  int hi = getByte(bits,0);
  int hd = getByte(bits, 1);
  int ti = getByte(bits, 2);
  int td = getByte(bits, 3);
  int csum = getByte(bits, 4);

  byte csum_check = hi +  hd + ti + td;
  if (csum_check != csum) {
    // The checksum doesn't tally. Something is
    // wrong.
    return false;
  }
  curr_temp_ = ti * 100 + td;
  curr_hum_ = hi * 100 + hd;
  return true;
}

int TempHumSensor::getTemperaturex100() {
  return curr_temp_;
}

int TempHumSensor::getHumidityx100() {
  return curr_hum_;
}
