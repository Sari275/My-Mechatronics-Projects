/**
 * Page: Positional versus Continuous Servos
 * Section: Code to Control Continuous Servos
 * https://www.makerguides.com/positional-vs-continuous-sg90-servos/
 */

#include <Servo.h>

const int maxspeed = 700;
const int stop = 1500;
Servo servo;

void setup() {
  servo.attach(9); // PIN 9
}

void rotate(int speed, int wait) {
  speed = map(speed, -100, +100, stop - maxspeed, stop + maxspeed);
  servo.writeMicroseconds(speed);
  delay(wait);
}

void loop() {
  rotate(0, 1000);
  rotate(-100, 1000); // counter-clockwise
  rotate(+100, 1000); // clockwise
}