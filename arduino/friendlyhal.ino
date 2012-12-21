/* The firmware that runs on the Arduino component of
 * the FriendlyHal project. It creates an event stream
 * from various sensors and error events and passes them
 * over the serial interface to the host board.
 */

#include "HardwareSerial.h"
#include "TempHumSensor.h"
// #include "PIRSensor.h"

#define TEMP_HUM_SENSOR 9
#define PIR_SENSOR 10

TempHumSensor tempHumSensor;
//PIRSensor pirSensor;

void setup() {
     tempHumSensor.init(TEMP_HUM_SENSOR);
  //   pirSensor.init(PIR_SENSOR);

     Serial.begin(9600);
}

void loop() {
     Serial.println("Hello!");
}
