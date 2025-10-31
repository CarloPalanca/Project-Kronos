#define actDelay 10
#define revDelay 400
#define moveSpeed     127*1.0                   // ADJUSTABLE - <1.0 for fakeout | =1.0 for competition | >1.0 for strong robots
#define forwardRange  60                        // ADJUSTABLE - 40 for close | 60 default | 80 long | 100 qualifying
#define sideRange     60                        // TEST - 60 OR 80
#define AIN_THRESHOLD 200                       // ADJUSTABLE - 200 for ASICUP | 70 100 FOR OLD RING
int RPWM1 = 9;   int RPWM2 = 5;
int LPWM1 = 10;  int LPWM2 = 6;
int REN1 = 11;   int REN2 = 7;
int LEN1 = 12;   int LEN2 = 8;
int trig1 = 4;   int echo1 = 13;
int trig2 = A1;  int echo2 = A0;
int trig3 = 2;   int echo3 = 3;
int trig4 = A3;  int echo4 = A2;
void setup() {
  pinMode(trig1, OUTPUT);  pinMode(echo1, INPUT);
  pinMode(trig2, OUTPUT);  pinMode(echo2, INPUT);
  pinMode(trig3, OUTPUT);  pinMode(echo3, INPUT);
  pinMode(trig4, OUTPUT);  pinMode(echo4, INPUT);
  pinMode(LPWM1, OUTPUT);  pinMode(RPWM1, OUTPUT);
  pinMode(LPWM2, OUTPUT);  pinMode(RPWM2, OUTPUT);
  pinMode(LEN1, OUTPUT);   pinMode(LEN2, OUTPUT);
  pinMode(REN1, OUTPUT);   pinMode(REN2, OUTPUT);
  pinMode(A4, INPUT);      pinMode(A5, INPUT);
  analogWrite(LPWM1, moveSpeed); analogWrite(RPWM1, moveSpeed);
  analogWrite(LPWM2, moveSpeed); analogWrite(RPWM2, moveSpeed);
  delay(4500);
}
void loop() {
  int AINL_val = analogRead(A4); bool AINL = AINL_val > AIN_THRESHOLD;
  int AINR_val = analogRead(A5); bool AINR = AINR_val > AIN_THRESHOLD;
  long duration1, distance1; long duration3, distance3;
  long duration2, distance2; long duration4, distance4;
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
  digitalWrite(LEN1, HIGH); digitalWrite(LEN2, HIGH);
  digitalWrite(REN1, HIGH); digitalWrite(REN2, HIGH);
  //Serial.print(AINL_val); Serial.print(" "); Serial.println(AINR_val);
  if (!AINL && !AINR) { //00
    analogWrite(LPWM1, 0); analogWrite(RPWM1, moveSpeed);
    analogWrite(LPWM2, 0); analogWrite(RPWM2, moveSpeed);
    delay(revDelay);
  }
  else if (!AINL && AINR) { //01
    analogWrite(LPWM1, 0); analogWrite(RPWM1, moveSpeed);
    analogWrite(LPWM2, 0); analogWrite(RPWM2, moveSpeed);
    delay(revDelay);
  }
  else if (AINL && !AINR) { //10
    analogWrite(LPWM1, 0); analogWrite(RPWM1, moveSpeed);
    analogWrite(LPWM2, 0); analogWrite(RPWM2, moveSpeed);
    delay(revDelay);
  }
  else if (AINL && AINR) { //11
    if (distance3 < forwardRange) { //LEFT
      analogWrite(LPWM1, 0);         analogWrite(RPWM1, moveSpeed);
      analogWrite(LPWM2, moveSpeed); analogWrite(RPWM2, 0);
    }
    else if (distance2 < forwardRange) { //RIGHT
      analogWrite(LPWM1, moveSpeed); analogWrite(RPWM1, 0);
      analogWrite(LPWM2, 0);         analogWrite(RPWM2, moveSpeed);
    }
    else if (distance1 < forwardRange) { //ATTACK
      analogWrite(LPWM1, moveSpeed*1.2); analogWrite(RPWM1, 0);
      analogWrite(LPWM2, moveSpeed*1.2); analogWrite(RPWM2, 0);
    }
    else if (distance4 < forwardRange) { //ATTACK
      analogWrite(LPWM1, moveSpeed*1.2); analogWrite(RPWM1, 0);
      analogWrite(LPWM2, moveSpeed*1.2); analogWrite(RPWM2, 0);
    }
    else { //STOP
      analogWrite(LPWM1, moveSpeed); analogWrite(RPWM1, moveSpeed);
      analogWrite(LPWM2, moveSpeed); analogWrite(RPWM2, moveSpeed);
    }
    delay(actDelay);
  }
}
