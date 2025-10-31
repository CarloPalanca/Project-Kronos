#include <IRremote.h>
void IR_INIT();
void MAIN_PROGRAM();
void stop();
IRrecv IR(1);
byte flag = 3;
int R_PWM2 = 9;   int R_PWM1 = 5;
int L_PWM2 = 10;  int L_PWM1 = 6;
int R_EN2 = 11;   int R_EN1 = 7;
int L_EN2 = 12;   int L_EN1 = 8;
int trig1 = 4;   int echo1 = 13;
int trig2 = A1;  int echo2 = A0;
int trig3 = 2;   int echo3 = 3;
int trig4 = A3;  int echo4 = A2;
long duration1, distance1;
long duration2, distance2;
long duration3, distance3;
long duration4, distance4;
#define moveSpeed 127                  // ADJUSTABLE - <1.0 for fakeout | =1.0 for competition | >1.0 for strong robots
#define maxRange  60                   // ADJUSTABLE - 60 default
#define threshold 400                  // ADJUSTABLE - 200 for ASICUP
void setup() {
  pinMode(trig1, OUTPUT);   pinMode(echo1, INPUT);
  pinMode(trig2, OUTPUT);   pinMode(echo2, INPUT);
  pinMode(trig3, OUTPUT);   pinMode(echo3, INPUT);
  pinMode(trig4, OUTPUT);   pinMode(echo4, INPUT);
  pinMode(R_EN1, OUTPUT);   pinMode(L_EN1, OUTPUT);
  pinMode(R_EN2, OUTPUT);   pinMode(L_EN2, OUTPUT);
  pinMode(R_PWM1, OUTPUT);  pinMode(L_PWM1, OUTPUT);
  pinMode(R_PWM2, OUTPUT);  pinMode(R_PWM2, OUTPUT);
  pinMode(A4, INPUT);       pinMode(A5, INPUT);
  digitalWrite(L_EN1, HIGH);  digitalWrite(R_EN1, HIGH);
  digitalWrite(L_EN2, HIGH);  digitalWrite(R_EN2, HIGH);
  IR.enableIRIn();
}
void loop() {
  IR_INIT();
  if (flag == 1) {
    Serial.println("go in");
    MAIN_PROGRAM();
    IR_INIT();
    IR.resume();
  } else if (flag == 0) {
    Serial.println("stop out");
    stop();
    IR_INIT();
    IR.resume();
  }
}
void IR_INIT() {
  if (IR.decode()) {
    if (IR.decodedIRData.decodedRawData == 0xBA45FF00) { flag = 1; } 
    else if (IR.decodedIRData.decodedRawData == 0xB946FF00) { flag = 0; }
  }
}
void MAIN_PROGRAM() {
  digitalWrite(trig1, LOW);   delayMicroseconds(2);
  digitalWrite(trig1, HIGH);  delayMicroseconds(10);  digitalWrite(trig1, LOW);
  duration1 = pulseIn(echo1, HIGH); distance1 = (duration1 / 2) / 29.1;
  digitalWrite(trig2, LOW);   delayMicroseconds(2);
  digitalWrite(trig2, HIGH);  delayMicroseconds(10);  digitalWrite(trig2, LOW);
  duration2 = pulseIn(echo2, HIGH); distance2 = (duration2 / 2) / 29.1;
  digitalWrite(trig3, LOW);   delayMicroseconds(2);
  digitalWrite(trig3, HIGH);  delayMicroseconds(10);  digitalWrite(trig3, LOW);
  duration3 = pulseIn(echo3, HIGH); distance3 = (duration3 / 2) / 29.1;
  digitalWrite(trig4, LOW);   delayMicroseconds(2);
  digitalWrite(trig4, HIGH);  delayMicroseconds(10);  digitalWrite(trig4, LOW);
  duration4 = pulseIn(echo4, HIGH); distance4 = (duration4 / 2) / 29.1;
  if ((SensorL < threshold) && (SensorR < threshold) ) { //00 
    analogWrite(R_PWM1, moveSpeed);     analogWrite(L_PWM1, 0);
    analogWrite(R_PWM2, moveSpeed);     analogWrite(L_PWM2, 0);
  } else if ((SensorL < threshold) && (SensorR > threshold) ) { //01 
    analogWrite(R_PWM1, moveSpeed);     analogWrite(L_PWM1, 0);
    analogWrite(R_PWM2, moveSpeed);     analogWrite(L_PWM2, 0);
  } else if ((SensorL > threshold) && (SensorR < threshold) ) { //10 
    analogWrite(R_PWM1, moveSpeed);     analogWrite(L_PWM1, 0);
    analogWrite(R_PWM2, moveSpeed);     analogWrite(L_PWM2, 0);
  } else if ((SensorL > threshold) && (SensorR > threshold) ) { //11 
    if (distance3 < maxRange) { // left
      analogWrite(L_PWM1, 0);           analogWrite(L_PWM2, moveSpeed);
      analogWrite(R_PWM1, moveSpeed);   analogWrite(R_PWM2, 0);
    } else if (distance1 < maxRange) {
      analogWrite(L_PWM1, moveSpeed);   analogWrite(L_PWM2, moveSpeed);
      analogWrite(R_PWM1, 0);           analogWrite(R_PWM2, 0);
    } else if (distance2 < maxRange) { //right
      analogWrite(L_PWM1, moveSpeed);   analogWrite(L_PWM2, 0);
      analogWrite(R_PWM1, 0);           analogWrite(R_PWM2, moveSpeed);
    } else if (distance4 < maxRange) {
      analogWrite(L_PWM1, moveSpeed);   analogWrite(L_PWM2, moveSpeed);
      analogWrite(R_PWM1, 0);           analogWrite(R_PWM2, 0);
    } else {
      analogWrite(L_PWM1, moveSpeed);   analogWrite(L_PWM2, moveSpeed);
      analogWrite(R_PWM1, moveSpeed);   analogWrite(R_PWM2, moveSpeed);
    }
    IR_INIT();
    IR.resume();
  }
}
void stop() {
  analogWrite(L_PWM1, moveSpeed);  analogWrite(L_PWM2, moveSpeed);
  analogWrite(R_PWM1, moveSpeed);  analogWrite(R_PWM2, moveSpeed);
}
