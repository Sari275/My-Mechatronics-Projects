/*
 * Created by ArduinoGetStarted.com
 * This example code is in the public domain
 * Tutorial page: https://arduinogetstarted.com/tutorials/arduino-button-servo-motor
 */

#include <Servo.h>


const int BUTTON_PIN_1 = 6;   // Arduino pin connected to button's pin
const int SERVO_PIN_1  = A3;  // Arduino pin connected to servo motor's pin

Servo servo_1;                // create servo object to control a servo

int angle_1 = 0;              // the current angle of servo motor. Try changing angle to be a variable and add 5 degrees everytime button is pressed once 180 deg is reached reset to 0
int lastButtonState_1;        // the previous state of button
int currentButtonState_1;     // the current state of button


void setup() {
  Serial.begin(9600);                   // initialize serial

  pinMode(BUTTON_PIN_1, INPUT_PULLUP);  // set arduino pin to input pull-up mode
  servo_1.attach(SERVO_PIN_1);          // attaches the servo on pin 9 to the servo object

  servo_1.write(angle_1);
  currentButtonState_1 = digitalRead(BUTTON_PIN_1);
}

void loop() {
  lastButtonState_1    = currentButtonState_1;      // save the last state
  currentButtonState_1 = digitalRead(BUTTON_PIN_1); // read new state
  
  if(lastButtonState_1 == HIGH && currentButtonState_1 == LOW) {
    Serial.println("The button 1 is pressed");

    if(angle_1 == 0)
      angle_1 = 5;
    else if(angle_1 < 180)
      angle_1 = angle_1 + 5;

    if(angle_1 >= 180)
      angle_1 = 0;
    servo_1.write(angle_1);
    Serial.print("Servo 1 angle: ");
    Serial.println(angle_1);
  }
}


