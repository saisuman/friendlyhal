/* The firmware that runs on the Arduino component of
 * the FriendlyHal project. It creates an event stream
 * from various sensors and error events and passes them
 * over the serial interface to the host board.
 */

#include "Arduino.h"
#include "HardwareSerial.h"
#include "TempHumSensor.h"
#include "PIRSensor.h"

#define EVENT_LOOP_DELAY_MS 1000

#define PIN_TEMP_HUM_SENSOR 9
#define PIN_PIR_SENSOR 10
#define PIN_CAMERA_MOTOR_ELEVATION 14

#define SOURCE_TEMP_HUM_SENSOR "temp_hum_sensor"
#define SOURCE_PIR_SENSOR "pir_sensor"

#define EVENT_TYPE_DATA "data"
#define EVENT_TYPE_INIT "init"
#define EVENT_TYPE_ERROR "error"

#define DATA_TEMPERATURE "temperature"
#define DATA_HUMIDITY "humidity"
#define DATA_MOTION "motion"

TempHumSensor tempHumSensor;
PIRSensor pirSensor;

void dataEvent(const char *dataType, int value) {
  Serial.print(EVENT_TYPE_DATA);
  Serial.print(":");
  Serial.print(dataType);
  Serial.print(",");
  Serial.println(value);
}

void errorEvent(const char *errMsg) {
  Serial.print(EVENT_TYPE_ERROR);
  Serial.print(":");
  Serial.println(errMsg);
}

void initEvent(const char *msg) {
  Serial.print(EVENT_TYPE_INIT);
  Serial.print(":");
  Serial.println(msg);
}

void setup() {
  Serial.begin(9600);
  
  tempHumSensor.init(PIN_TEMP_HUM_SENSOR);
  initEvent("Initialised temperature/humidity sensor.");
  pirSensor.init(PIN_PIR_SENSOR);
  initEvent("Initialised PIR sensor.");
}

// This is used to buffer input from Serial when it can't
// all be read together.
unsigned char inputBuf[1024];
int bytesInBuffer = 0;

void loop() {
  
  delay(EVENT_LOOP_DELAY_MS);

  if (!tempHumSensor.refresh()) {
    errorEvent(SOURCE_TEMP_HUM_SENSOR);
  } else {
    int temperature = tempHumSensor.getTemperaturex100();
    int humidity = tempHumSensor.getHumidityx100();
    dataEvent(DATA_TEMPERATURE, temperature);
    dataEvent(DATA_HUMIDITY, humidity);
  }

  if (pirSensor.motionDetected()) {
    dataEvent(DATA_MOTION, 1);
  }
}
