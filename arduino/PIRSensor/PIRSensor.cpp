#include "Arduino.h"
#include "PIRSensor.h"

PIRSensor::PIRSensor() {}
PIRSensor::~PIRSensor() {}

void PIRSensor::init(int pin) {
  pin_ = pin;
  pinMode(pin_, INPUT);
}

bool PIRSensor::motionDetected() {
  return digitalRead(pin_) == HIGH;
}
