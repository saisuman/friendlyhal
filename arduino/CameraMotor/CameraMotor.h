/*
  Library for controlling the panning of the camera.
 */
#ifndef camera_motor_h
#define camera_motor_h

#include "Arduino.h"
#include "Servo.h"

#define INCREMENT_DEGREES 2
#define INITIAL_POSITION_DEGREES 90

class CameraMotor {
 public:
  // Sets up the camera motor, given a pin to which
  // the servo is attached.
  CameraMotor();
  void init(int pin);
  // Pans the camera up a little bit.
  void panUp();
  // Pans the camera down a little bit.
  void panDown();

 private:
  // Moves the motor to the specified postion.
  void apply(); 
  Servo servo_motor_;
  int current_angle_;
};

#endif
