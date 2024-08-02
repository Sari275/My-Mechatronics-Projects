#include <Servo.h>
#include "Adafruit_seesaw.h"
#include <seesaw_neopixel.h>


/* Arcade Button declarations */
#define  DEFAULT_I2C_ADDR 0x3A
#define NUM_BUTTONS 4
int arcadeSwitch[NUM_BUTTONS] = {18, 19, 20, 2};      // PA01, PA02, PA03, PA06
int arcadePWM[NUM_BUTTONS] = {12, 13, 0, 1};          // PC00, PC01, PA04, PA05

Adafruit_seesaw ss;

void arcadeButtonSetup() {
  Serial.println(F("Adafruit PID 5296 I2C QT 4x LED Arcade Buttons test!"));

  if (!ss.begin(DEFAULT_I2C_ADDR)) {
    Serial.println(F("seesaw not found!"));
    while(1) {
      delay(10);
    }
  }

  uint16_t pid;
  uint8_t year, mon, day;
  
  ss.getProdDatecode(&pid, &year, &mon, &day);
  Serial.print("seesaw found PID: ");
  Serial.print(pid);
  Serial.print(" datecode: ");
  Serial.print(2000+year); Serial.print("/"); 
  Serial.print(mon); Serial.print("/"); 
  Serial.println(day);

  if (pid != 5296) {
    Serial.println(F("Wrong seesaw PID"));
    while (1) {
      delay(10);
    }
  }

  Serial.println(F("seesaw started OK!"));
  for (int i=0; i<NUM_BUTTONS; i++) {
    ss.pinMode(arcadeSwitch[i], INPUT_PULLUP);
    ss.analogWrite(arcadePWM[i], 127);
  }
}

/* Servo declarations */
#define MAX_ANGLE 155

#define NUM_LANES 2
const int servoPin[NUM_LANES] = {A0, A1};              // Arduino pin connected to servo motor's pin
int angle[NUM_LANES] = {0, 0};                         // the current angle of servo motor
int lastButtonState[NUM_LANES];                        // the previous state of button
int currentButtonState[NUM_LANES];                     // the current state of button

// #define NUM_LANES 4
// const int servoPin[NUM_LANES] = {A0, A1, A2, A3};   // Arduino pin connected to servo motor's pin
// int angle[NUM_LANES] = {0, 0, 0, 0};                // the current angle of servo motor
// int lastButtonState[NUM_LANES];                     // the previous state of button
// int currentButtonState[NUM_LANES];                  // the current state of button

Servo servo0;
Servo servo1;
Servo servo2;
Servo servo3;

Servo servoSelection(int i) {
  switch (i) {
    case 0:
      return servo0;
      break;
    case 1:
      return servo1;
      break;
    case 2:
      return servo2;
      break;
    case 3:
      return servo3;
      break;
  }
}

void servoSetup() {
  Servo currentServo;

  for (int i=0; i<NUM_LANES; i++) {
    currentServo = servoSelection(i);
    currentServo.attach(servoPin[i]);                     // attaches the servo on pin 9 to the servo object
    currentServo.write(angle[i]);
    currentButtonState[i] = ss.digitalRead(arcadeSwitch[i]);
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);        // wait until serial port is opened
  }

  arcadeButtonSetup();
  servoSetup();
}


void buttonStateUpdate(int i) {
    lastButtonState[i]    = currentButtonState[i];      // save the last state
    currentButtonState[i] = ss.digitalRead(arcadeSwitch[i]);  // read new state
}

uint8_t incr = 0;

void arcadeSwitchCheck(int i) {
  if (!ss.digitalRead(arcadeSwitch[i])) {
    String str = String("Switch ") + (i+1) + " pressed";
    Serial.println(str);

    ss.analogWrite(arcadePWM[i], incr);
    incr += 5;
  } else {
    ss.analogWrite(arcadePWM[i], 0);
  }
}

void buttonPress(int i) {
  String str = String("Button ") + (i+1) + " pressed";
  Serial.println(str);

  angle[i] += 5;
  if(angle[i] > MAX_ANGLE) {
    angle[i] = 0;
  }

  str = String("Servo ") + (i+1) + " angle: " + angle[i];
  Serial.println(str);

  Servo currentServo = servoSelection(i);
  currentServo.write(angle[i]);
}

void loop() {
  for (int i=0; i < NUM_LANES; i++) {
    buttonStateUpdate(i);

    arcadeSwitchCheck(i);
    if(lastButtonState[i] == HIGH && currentButtonState[i] == LOW) {
      buttonPress(i);
    }
  }
}
