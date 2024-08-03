int outputPin=6;
int outputPin1=5;
double error,output,w;

//int setpoint =100;
unsigned long lastTime;
double errSum, lastErr,lastdegrees;

//double kp=2, ki=0.5, kd=0.3 ;
double kp=0.7, ki=0.0001, kd=.21 ; // to change kp, ki, kd in software change the values on this line
#include<LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 13, 4, 3, 2);// Creates an LCD object. Parameters: (rs, enable, d4, d5, d6, d7)

void setup() {
  // put setup code here, to run once:
 Serial.begin(9600);
 lcd.begin(16,2);
  
}

void loop() {
 
  // put main code here, to run repeatedly:
  int sensorValue = analogRead(A3);
  // print out the values being read:
  //Serial.println(sensorValue);
  float degrees=(sensorValue/2.86);
  int pot_sp=analogRead(A5);
  float pot_kp=analogRead(A1);
  float pot_kd=analogRead(A2);
  float pot_ki=analogRead(A4);
  int setpoint=map(pot_sp,0,1023,0,360);
  kp=mapfloat(pot_kp,0,1023,0,1); //comment if commanding via software / uncomment if commanding via pot 
  kd=mapfloat(pot_kd,0,1023,0,1);//comment if commanding via software / uncomment if commanding via pot 
  ki=mapfloat(pot_ki,0,1023,0,.0001);//comment if commanding via software/ uncomment if commanding via pot 
  
  Serial.print("kp: ");
  Serial.print(kp);
  Serial.print(",ki: ");
  Serial.print(ki,5);
  Serial.print(",kd: ");
  Serial.print(kd);
  Serial.print(",Current Angle: ");
  Serial.print(degrees);
  

  lcd.setCursor(0,0);
  lcd.print("DA:" + String(setpoint));
  lcd.print(",CA:" + String(degrees,0)+",p =");
  lcd.setCursor(0,1);
  lcd.print(String(kp,1)+",i="+String(ki,1)+",d="+String(kd,1));

  /*How long since last calculated*/
  unsigned long now = millis();
  double timeChange = (double)(now - lastTime);

  /*Compute all the working error variables*/
  double error = setpoint - degrees;
  errSum += (error * timeChange);
  double dErr = (error - lastErr) / timeChange;

  w = (degrees-lastdegrees)/ timeChange ; 

  /*Compute PID Output*/
  output = kp * error + ki * errSum + kd * dErr;

  /*Remember some variables for next time*/
  lastdegrees = degrees;
  lastErr = error;
  lastTime = now;
  

  //Serial.print(" , Output of Controller : ");
  //Serial.println(output); //diff-pwm for plotting
  //delay(1); // delay in between reads for stability
  //Serial.print(now);
 // Serial.print(",");
  //Serial.println(w);
  //Serial.print(",");
  //Serial.println(degrees);

 
  analogWrite(outputPin, 153+output); //60% + error/output this fan (X (error)is the differential)
  analogWrite(outputPin1,153-output); // 60% - error/output the other fan
  // equals 60% PWM with a 20% differential
  // read the input on analog pin 0:

}
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
