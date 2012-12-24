#include "CameraMotor.h"


CameraMotor::CameraMotor() {}

void CameraMotor::init(int pin) {
  servo_motor_.attach(pin);
  current_angle_ = INITIAL_POSITION_DEGREES;
  apply();
}

void CameraMotor::panUp() {
  current_angle_ += INCREMENT_DEGREES;
  apply();
}

void CameraMotor::panDown() {
  current_angle_ -= INCREMENT_DEGREES;
  apply();
}

void CameraMotor::apply() {
  current_angle_ = min(max(10, current_angle_), 160); 
  servo_motor_.write(current_angle_); 
}


