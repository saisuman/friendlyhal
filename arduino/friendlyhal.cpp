/* The firmware that runs on the Arduino component of
 * the FriendlyHal project. It creates an event stream
 * from various sensors and error events and passes them
 * over the serial interface to the host board.
 */

#include "Arduino.h"
#include "HardwareSerial.h"
#include "TempHumSensor.h"
#include "PIRSensor.h"
#include "CameraMotor.h"

#define EVENT_LOOP_DELAY_MS 1000

#define PIN_TEMP_HUM_SENSOR 9
#define PIN_PIR_SENSOR 8
#define PIN_CAMERA_MOTOR_ELEVATION 14

#define SOURCE_TEMP_HUM_SENSOR "temp_hum_sensor"
#define SOURCE_PIR_SENSOR "pir_sensor"

#define EVENT_TYPE_DATA "data"
#define EVENT_TYPE_INIT "init"
#define EVENT_TYPE_ERROR "error"

#define DATA_TEMPERATURE "temperature"
#define DATA_HUMIDITY "humidity"
#define DATA_MOTION "motion"

#define CMD_PAN_UP "panup"
#define CMD_PAN_DOWN "pandown"

TempHumSensor tempHumSensor;
PIRSensor pirSensor;
CameraMotor cameraMotor;

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

// This is used to buffer input from Serial when it can't
// all be read together.
#define INPUT_BUFSIZE 1024
byte inputBuf[INPUT_BUFSIZE];
int bytesInBuffer = 0;

void setup() {
  Serial.begin(9600);  
  tempHumSensor.init(PIN_TEMP_HUM_SENSOR);
  initEvent("Initialised temperature/humidity sensor.");
  pirSensor.init(PIN_PIR_SENSOR);
  initEvent("Initialised PIR sensor.");
  cameraMotor.init(PIN_CAMERA_MOTOR_ELEVATION);
  initEvent("Initialised camera motor.");
}

bool cmp(const char *str1, const char *str2) {
  const char *c1 = str1, *c2 = str2;
  for (; *c1 != 0 && *c2 != 0; ++c1, ++c2) {
    if (*c1 != *c2) {
      return false;
    }
  }
  return *c1 == *c2;
}

void parseAndProcessCommand() {
  char *newlineIndex = (char*)inputBuf;
  for (; *newlineIndex != '\r'; newlineIndex++) {
    if (*newlineIndex == '\0') {
      newlineIndex = NULL;
      break;
    }
  }

  if (newlineIndex == NULL) {
    return;
  }

  char commandBuf[1024];
  int index = 0;
  const char *c = (char*)inputBuf;
  for (; c != newlineIndex && *c != 0; ++c) {
    commandBuf[index++] = *c;
  }
  commandBuf[index] = 0;
  // We're at the newline now. Skip over.
  ++c;
  // Handle the trailing part. Assuming zero-termination
  // in the original buffer.
  bytesInBuffer = 0;
  while (*c != 0) {
    inputBuf[bytesInBuffer++] = *(c++);
  }
  inputBuf[bytesInBuffer] = 0;

  // Now, is there a command?
  if (cmp(commandBuf, CMD_PAN_UP)) {
    cameraMotor.panUp();
    Serial.println("panned up");
  } else if (cmp(commandBuf, CMD_PAN_DOWN)) {
    cameraMotor.panDown();
    Serial.println("panned down");
  } else {
    errorEvent("Invalid command.");
  }
}

void loop() {
  
  delay(EVENT_LOOP_DELAY_MS);

  if (!tempHumSensor.refresh()) {
    errorEvent("Temperature/Humidity sensor error.");
  } else {
    int temperature = tempHumSensor.getTemperaturex100();
    int humidity = tempHumSensor.getHumidityx100();
    dataEvent(DATA_TEMPERATURE, temperature);
    dataEvent(DATA_HUMIDITY, humidity);
  }

  if (pirSensor.motionDetected()) {
    dataEvent(DATA_MOTION, 1);
  }

  // Now we'll read up whatever we can.
  int availableBytes = Serial.available();
  if (availableBytes <= 0) {
    return;
  }

  int incomingByte = -1;
  while ((incomingByte = Serial.read()) != -1 &&
	 bytesInBuffer < INPUT_BUFSIZE - 1) {
    inputBuf[bytesInBuffer++] = (byte) incomingByte;
  }
  inputBuf[bytesInBuffer] = 0;
  parseAndProcessCommand();
}
