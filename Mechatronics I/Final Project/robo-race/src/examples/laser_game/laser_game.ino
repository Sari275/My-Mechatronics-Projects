/**
 * How to play:
 * Goal of the game is to shoot as many alien spaceships as possible.
 * At game start you have 5 shots. One of 5 red LEDs ("spaceship") lit randomly for a short
 * time and you have to shoot this spaceship as fast as possible. If you do not hit the
 * spaceship fast enough you loose and the game is over. After you hit the spaceship the
 * LED will go off. If you run out of shots the game is over, but for every successfully
 * hit spaceship you gain one more shot.
 */
#include <Wire.h> 
// #include <LiquidCrystal_I2C.h>
#include <LiquidCrystal.h>

// #define PIN_PIEZO 6

#define AMOUNT_SPACESHIPS 4
#define AMOUNT_SHOTS_INIT 5

#define MAX_SPACESHIP_DELAY  2000 // in ms
#define MIN_SPACESHIP_DELAY   250 // in ms
#define SPACESHIP_DELAY_STEP  100 // in ms

#define TONE_SHOT  1
#define TONE_END_1 2
#define TONE_END_2 3

// LiquidCrystal_I2C lcd(0x27,16,2);
const int rs = 8, en = 9, d4 = 10, d5 = 11, d6 = 12, d7 = 13;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

bool gameRunning;
byte pinLEDs[AMOUNT_SPACESHIPS] = {4,5,6,7};
byte pinButtons[AMOUNT_SPACESHIPS] = {30, 31, 32, 33};
int activeSpaceship;
byte shots;
unsigned int spaceshipDelay;
unsigned int score;
unsigned long shipAppearance;

void setup()
{
  Serial.begin(9600);

  lcd.begin(16,2);
  // lcd.begin();
  // lcd.setCursor(1,0);
  lcd.print(" Laser Shooter");
  lcd.setCursor(0,1);
  lcd.print("    mircemk");
  
  // randomSeed(analogRead(0));
  randomSeed(0);
  // pinMode(PIN_PIEZO, OUTPUT);
  for (byte i = 0; i < AMOUNT_SPACESHIPS; i++) {
    pinMode(pinLEDs[i], OUTPUT);
    digitalWrite(pinLEDs[i], LOW);
    pinMode(pinButtons[i], INPUT_PULLUP);
  }

  spaceshipDelay = MAX_SPACESHIP_DELAY;
  gameRunning = true;
  activeSpaceship = -1;
  score = 0;
  shots = AMOUNT_SHOTS_INIT;
  initAnimation();
}

void loop()
{
  if (gameRunning) {
    updateSpaceships();
  }
  checkShoot();
}

void updateSpaceships()
{
  if (activeSpaceship > -1) {
    if ((millis() - shipAppearance) > spaceshipDelay) {
      endGame();
    }
    return;
  }

  // delay between two spaceships
  delay(random(500, 3000));

  activeSpaceship = random(0, AMOUNT_SPACESHIPS);
  digitalWrite(pinLEDs[activeSpaceship], HIGH);
  shipAppearance = millis();
  if (spaceshipDelay > (MIN_SPACESHIP_DELAY + SPACESHIP_DELAY_STEP)) {
    spaceshipDelay -= SPACESHIP_DELAY_STEP;
  }
}

void checkShoot()
{
  if (activeSpaceship == -1) {
    return;
  }
  for (byte i = 0; i < AMOUNT_SPACESHIPS; i++) {
    if (digitalRead(pinButtons[i]) == HIGH) {
      continue;
    }
    Serial.println(pinButtons[i]);
    shots--;
    if (activeSpaceship == i) {
      // player hit spaceship
      digitalWrite(pinLEDs[activeSpaceship], LOW);
      activeSpaceship = -1;
      score++;
      shots++;
      // tone(PIN_PIEZO, 400, 50);
      return;
    }
    if (shots <= 0) {
      endGame();
      return;
    }
  }
}

void initAnimation()
{
  Serial.println("Incoming alien ships...");
  lcd.setCursor(0,0);
  lcd.print("Incoming ships  ");
  lcd.setCursor(0,1);
  lcd.print("  * * * * * *   ");
  for (byte i = 0; i < 9; i++) {
    for (byte j = 0; j < AMOUNT_SPACESHIPS; j++) {
      digitalWrite(pinLEDs[j], i%2);
    }
    delay(250);
    // tone(PIN_PIEZO, 1220 - (i%2 * 50));
    delay(300);
    // noTone(PIN_PIEZO);
  }
}

void endGame()
{
  gameRunning = false;
  Serial.println("GAME OVER");
  lcd.setCursor(0,0);
  lcd.print("   GAME OVER    ");
  lcd.setCursor(1,1);
  lcd.print("Final score: ");
  lcd.print(score);
  Serial.println("Final score: " + String(score));
  
  for (byte i = 0; i < 6; i++) {
    for (byte j = 0; j < AMOUNT_SPACESHIPS; j++) {
      digitalWrite(pinLEDs[j], i%2);
    }
    // tone(PIN_PIEZO, 660 - (i%2 * 50));
    delay(300);
    // noTone(PIN_PIEZO);
  }
  for (byte i = 0; i < AMOUNT_SPACESHIPS; i++) {
    digitalWrite(pinLEDs[i], LOW);
  }
}