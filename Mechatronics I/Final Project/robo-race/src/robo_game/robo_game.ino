/**
 * How to play:
 * Goal of the game is to move you robot to the finish line first. 
 * At game start all robots are at the starting line. 
 * Press the button to move the robot forward.
 * First one to the end wins.
 */

#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <seesaw_neopixel.h>
#include "Adafruit_seesaw.h"


#define NUM_LANES 4

/* LCD Declarations */
const int rs = 8, en = 9, d4 = 10, d5 = 11, d6 = 12, d7 = 13;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

/* LED Declarations */
// byte ledPin[NUM_LANES] = {2, 3};
byte ledPin1[NUM_LANES] = {3, 4, 5, 6};
byte ledPin2[NUM_LANES] = {22, 24, 26, 28};

/* Button Declarations */
// byte buttonPin[NUM_LANES] = {14, 16};
byte buttonPin[NUM_LANES] = {14, 15, 16, 17};

/* Arcade Button declarations */
#define  NUM_BUTTONS 4
#define  DEFAULT_I2C_ADDR 0x3A
int arcadeSwitch[NUM_BUTTONS] = {18, 19, 20, 2};      // PA01, PA02, PA03, PA06
int arcadePWM[NUM_BUTTONS] = {12, 13, 0, 1};          // PC00, PC01, PA04, PA05
Adafruit_seesaw ss;

/* Servo declarations */
#define MAX_ANGLE 155
// const int servoPin[NUM_LANES] = {A0, A2};           // Arduino pin connected to servo motor's pin
// int angle[NUM_LANES] = {0, 0};                      // the current angle of servo motor
const int servoPin[NUM_LANES] = {A0, A2, A4, A6};   // Arduino pin connected to servo motor's pin
int angle[NUM_LANES] = {0, 0, 0, 0};                // the current angle of servo motor
int lastButtonState[NUM_LANES];                     // the previous state of button
int currentButtonState[NUM_LANES];                  // the current state of button

/* Buzzer Declarations */
#define PIN_PIEZO 7

/* Coin Declarations */
#define COIN_PIN 2
const int coinInt = 0;
volatile float coinsValue = 0.0;
int coinsChange = 0;


void lcdSetup() {
  Serial.println("LCD Setup");
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Robo Race");
  lcd.setCursor(0,1);
  lcd.print("Sari, Vince");
}

void flashLEDs(int times, bool sound) {
  for (int i = 0; i < times; i++) {
    for (int j = 0; j < NUM_LANES; j++) {
      digitalWrite(ledPin1[j], i%2);
      digitalWrite(ledPin2[j], i%2);
    }

    delay(250);
    if (sound) {
      tone(PIN_PIEZO, 1220 - (i%2 * 50));
    }

    delay(300);
    if (sound) {
      notone(PIN_PIEZO);
    }
  }
}

void ledSetup() {
  Serial.println("LED Setup");
  for (byte i = 0; i < NUM_LANES; i++) {
    pinMode(ledPin1[i], OUTPUT);
    digitalWrite(ledPin1[i], LOW);

    pinMode(ledPin2[i], OUTPUT);
    digitalWrite(ledPin2[i], LOW);
  }

  flashLEDs(5, false);
}

void buttonSetup() {
  Serial.println("Button Setup");
  for (byte i = 0; i < NUM_LANES; i++) {
    pinMode(buttonPin[i], INPUT_PULLUP);
  }
}

void arcadeButtonSetup() {
  Serial.println("Arcade Button Setup");
  uint16_t pid;
  uint8_t year, mon, day;

  Serial.println(F("Adafruit PID 5296 I2C QT 4x LED Arcade Buttons test!"));
  if (!ss.begin(DEFAULT_I2C_ADDR)) {
    Serial.println(F("seesaw not found!"));
    while(1) {
      delay(10);
    }
  }

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
  for (byte i=0; i<NUM_BUTTONS; i++) {
    ss.pinMode(arcadeSwitch[i], INPUT_PULLUP);
    ss.analogWrite(arcadePWM[i], 127);
  }
}

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
  Serial.println("Servo Setup");
  Servo currentServo;

  for (byte i=0; i<NUM_LANES; i++) {
    currentServo = servoSelection(i);
    currentServo.attach(servoPin[i]);                     // attaches the servo on pin 9 to the servo object
    currentServo.write(angle[i]);
    // currentButtonState[i] = digitalRead(buttonPin[i]);
    currentButtonState[i] = ss.digitalRead(arcadeSwitch[i]);
  }
}

void buzzerSetup() {
  Serial.println("Buzzer Setup");
  pinMode(PIN_PIEZO, OUTPUT);
}

void coinInserted() {
  // As we set the Pulse to represent 5p or 5c we add this to the coinsValue
  coinsValue = coinsValue + 0.01;  

  // Flag that there has been a coin inserted
  coinsChange = 1;   
}

void coinSetup() {
  attachInterrupt(coinInt, coinInserted, RISING);
  // If coinInt goes HIGH (a Pulse), call the coinInserted function
  // An attachInterrupt will always trigger, even if your using delays
  pinMode(COIN_PIN, INPUT_PULLUP);
}

/* Game Declarations */
bool gameRunning;
int raceWinner;
void gameSetup() {
  Serial.println("Game Setup");
  gameRunning = false;
  raceWinner = -1;

  Servo currentServo;
  for (byte i=0; i<NUM_LANES; i++) {
    angle[i] = 0;
    currentServo = servoSelection(i);
    currentServo.write(angle[i]);
  }
}

void setup()
{
  Serial.begin(115200);
  randomSeed(1);

  lcdSetup();
  ledSetup();
  // buttonSetup();
  arcadeButtonSetup();
  servoSetup();
  buzzerSetup();
  coinSetup();

  gameSetup();
}


void loop() {
  if (gameRunning) {
    for (byte i=0; i < NUM_LANES; i++) {
      buttonStateUpdate(i);
      
      if(lastButtonState[i] == HIGH && currentButtonState[i] == LOW) {
        String str = String("Button ") + (i+1) + " pressed";
        Serial.println(str);
        updateServo(i);
      }
    }

    if (checkWinner()) {
      endGame(raceWinner);
      gameSetup();
    }
  } else {
    if (checkStart()) {
      startGame();
    }
  }

  // Reset coin detection
  coinsChange = 0;
}


void buttonStateUpdate(int i) {
    lastButtonState[i]    = currentButtonState[i];      // save the last state
    // currentButtonState[i] = digitalRead(buttonPin[i]);
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

bool checkStart() {
  if (coinsChange==1) {
    return true;
  }
  return false;
}

void startGame() {
  Serial.println("Start Race");

  flashLEDs(9, true);

  Serial.println("Ready...        ");
  lcd.setCursor(0,0);
  lcd.print("Ready...        ");
  lcd.setCursor(0,1);
  lcd.print("                ");
  delay(1000);

  flashLEDs(7, true);

  Serial.println("Set...          ");
  lcd.setCursor(0,0);
  lcd.print("Set...          ");
  lcd.setCursor(0,1);
  lcd.print("               ");
  delay(1000);

  flashLEDs(5, true);

  Serial.println("Go!!!           ");
  lcd.setCursor(0,0);
  lcd.print("Go!!!           ");
  lcd.setCursor(0,1);
  lcd.print("                ");

  flashLEDs(2, true);

  gameRunning = true;
}

void updateServo(int i) {
  angle[i] += 1;
  Servo currentServo = servoSelection(i);
  currentServo.write(angle[i]);

  String str = String("Servo ") + (i+1) + " angle: " + angle[i];
  Serial.println(str);
}

bool checkWinner() {
  for (byte i=0; i < NUM_LANES; i++) {
    if (angle[i] >= MAX_ANGLE) {
      raceWinner = i;
      return true;
    }
  }
  return false;
}

void endGame(int winner) {
  String str = "RACE OVER";
  Serial.println(str);
  lcd.setCursor(0,0);
  lcd.print("   " + str + "    ");

  str = " Robo Winner: " + String(winner+1);
  Serial.println(str);
  lcd.setCursor(0,1);
  lcd.print(str);

  for (byte i = 0; i < 20; i++) {
    digitalWrite(ledPin1[winner], i%2);
    digitalWrite(ledPin2[winner], i%2);
    tone(PIN_PIEZO, 660 - (i%2 * 50));
    delay(300);
    noTone(PIN_PIEZO);
  }
  digitalWrite(ledPin1[winner], LOW);
  digitalWrite(ledPin2[winner], LOW);
}
