/*
 *  Servo actuation for robo race
 */

#include <Servo.h>


#define NUM_LANES 2
const int buttonPin[NUM_LANES] = {14, 15};          // Arduino pin connected to button's pin
const int servoPin[NUM_LANES] = {A0, A2};         // Arduino pin connected to servo motor's pin
int angle[NUM_LANES] = {0, 0};                    // the current angle of servo motor

// #define NUM_LANES 4
// const int buttonPin[NUM_LANES] = {14, 15, 16, 17};    // Arduino pin connected to button's pin
// const int servoPin[NUM_LANES] = {A0, A1, A2, A3}; // Arduino pin connected to servo motor's pin
// int angle[NUM_LANES] = {0, 0, 0, 0};              // the current angle of servo motor


/* Button declarations */
int lastButtonState[NUM_LANES];                   // the previous state of button
int currentButtonState[NUM_LANES];                // the current state of button

void buttonSetup(int i) {
  Servo currentServo = servoSelection(i);

  pinMode(buttonPin[i], INPUT_PULLUP);                  // set arduino pin to input pull-up mode
  currentServo.attach(servoPin[i]);                     // attaches the servo on pin 9 to the servo object
  currentServo.write(angle[i]);
  currentButtonState[i] = digitalRead(buttonPin[i]);
}

/* Servo declarations */
#define MAX_ANGLE 155

Servo servo0;                                     // create servo object to control a servo
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

void setup() {
  Serial.begin(115200);                                   // initialize serial

  for (int i=0; i < NUM_LANES; i++) {
    buttonSetup(i);
  }
}


void buttonStateUpdate(int i) {
    lastButtonState[i]    = currentButtonState[i];      // save the last state
    currentButtonState[i] = digitalRead(buttonPin[i]);  // read new state
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
    
    if(lastButtonState[i] == HIGH && currentButtonState[i] == LOW) {
      buttonPress(i);
    }
  }
}



