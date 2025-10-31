// RIGHT DRIVER
#define RPWM2 5
#define LPWM2 6
#define REN2 7
#define LEN2 8
// LEFT DRIVER
#define RPWM1 9
#define LPWM1 10
#define REN1 11
#define LEN1 12

#define actDelay 10
#define maxRange 60

int trig1 = 4;   int echo1 = 13;  // LEFT FRONT SENSOR
int trig2 = A1;  int echo2 = A0;  // RIGHT FRONT SENSOR
int trig3 = 2;   int echo3 = 3;   // LEFT SIDE SENSOR
int trig4 = A3;  int echo4 = A2;  // RIGHT SIDE SENSOR

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
}

void loop() {
  int AINL = digitalRead(A4);  // analog input sensor L
  int AINR = digitalRead(A5);  // analog input sensor R
  long duration1, distance1;
  long duration2, distance2;
  long duration4, distance4;
  long duration3, distance3;

  // Read distance from ultrasonic sensors
  digitalWrite(trig1, LOW); delayMicroseconds(2); digitalWrite(trig1, HIGH); delayMicroseconds(10); digitalWrite(trig1, LOW);
  duration1 = pulseIn(echo1, HIGH); distance1 = (duration1 / 2) / 29.1;

  digitalWrite(trig2, LOW); delayMicroseconds(2); digitalWrite(trig2, HIGH); delayMicroseconds(10); digitalWrite(trig2, LOW);
  duration2 = pulseIn(echo2, HIGH); distance2 = (duration2 / 2) / 29.1;

  digitalWrite(trig3, LOW); delayMicroseconds(2); digitalWrite(trig3, HIGH); delayMicroseconds(10); digitalWrite(trig3, LOW);
  duration3 = pulseIn(echo3, HIGH); distance3 = (duration3 / 2) / 29.1;

  digitalWrite(trig4, LOW); delayMicroseconds(2); digitalWrite(trig4, HIGH); delayMicroseconds(10); digitalWrite(trig4, LOW);
  duration4 = pulseIn(echo4, HIGH); distance4 = (duration4 / 2) / 29.1;

  if (!AINL && !AINR) { // 00: reverse
    digitalWrite(LEN1, HIGH); digitalWrite(LEN2, HIGH);
    digitalWrite(REN1, HIGH); digitalWrite(REN2, HIGH);
    analogWrite(LPWM1, 0); analogWrite(LPWM2, 127);
    analogWrite(RPWM1, 0); analogWrite(RPWM2, 127);
    delay(actDelay);
  }
  else if (!AINL && AINR) { // 01: reverse
    digitalWrite(LEN1, HIGH); digitalWrite(LEN2, HIGH);
    digitalWrite(REN1, HIGH); digitalWrite(REN2, HIGH);
    analogWrite(LPWM1, 0); analogWrite(LPWM2, 127);
    analogWrite(RPWM1, 0); analogWrite(RPWM2, 127);
    delay(actDelay);
  }
  else if (AINL && !AINR) { // 10: reverse
    digitalWrite(LEN1, HIGH); digitalWrite(LEN2, HIGH);
    digitalWrite(REN1, HIGH); digitalWrite(REN2, HIGH);
    analogWrite(LPWM1, 0); analogWrite(LPWM2, 127);
    analogWrite(RPWM1, 0); analogWrite(RPWM2, 127);
    delay(actDelay);
  }
  else if (AINL && AINR) { // 11: engaged

    // ATTACK - front detection
    if (!(distance3 <= maxRange || distance3 >= 0) && 
        (distance1 <= maxRange || distance1 >= 0) && 
        (distance2 <= maxRange || distance2 >= 0) && 
        !(distance4 <= maxRange || distance4 >= 0)) {

      // Determine closest front sensor
      int frontDistance = min(distance1, distance2);

      int speedStart = 127;
      int speedEnd = 255;
      int rampStep = 10;
      int rampDelay;

      if (frontDistance > 40) {
        rampDelay = 30;
      } else if (frontDistance > 20) {
        rampDelay = 15;
      } else {
        speedStart = 255;  // Skip ramping
        speedEnd = 255;
        rampDelay = 0;
      }

      digitalWrite(LEN1, HIGH); digitalWrite(LEN2, HIGH);
      digitalWrite(REN1, HIGH); digitalWrite(REN2, HIGH);

      for (int spd = speedStart; spd <= speedEnd; spd += rampStep) {
        analogWrite(LPWM1, spd);  analogWrite(LPWM2, 0);
        analogWrite(RPWM1, spd);  analogWrite(RPWM2, 0);
        if (rampDelay > 0) delay(rampDelay);
      }
      delay(actDelay);
    }

    // LEFT TURN
    else if (!(distance3 <= maxRange || distance3 >= 0) && (distance1 <= maxRange || distance1 >= 0) && !(distance2 <= maxRange || distance2 >= 0) && !(distance4 <= maxRange || distance4 >= 0)) {
      digitalWrite(LEN1, HIGH); digitalWrite(LEN2, HIGH);
      digitalWrite(REN1, HIGH); digitalWrite(REN2, HIGH);
      analogWrite(LPWM1, 0); analogWrite(LPWM2, 127);
      analogWrite(RPWM1, 127); analogWrite(RPWM2, 0);
      delay(actDelay);
    }

    // RIGHT TURN
    else if (!(distance3 <= maxRange || distance3 >= 0) && !(distance1 <= maxRange || distance1 >= 0) && (distance2 <= maxRange || distance2 >= 0) && !(distance4 <= maxRange || distance4 >= 0)) {
      digitalWrite(LEN1, HIGH); digitalWrite(LEN2, HIGH);
      digitalWrite(REN1, HIGH); digitalWrite(REN2, HIGH);
      analogWrite(LPWM1, 127); analogWrite(LPWM2, 0);
      analogWrite(RPWM1, 0); analogWrite(RPWM2, 127);
      delay(actDelay);
    }

    // LEFT SIDE TURN
    else if ((distance3 <= maxRange || distance3 >= 0) && !(distance1 <= maxRange || distance1 >= 0) && !(distance2 <= maxRange || distance2 >= 0) && !(distance4 <= maxRange || distance4 >= 0)) {
      digitalWrite(LEN1, HIGH); digitalWrite(LEN2, HIGH);
      digitalWrite(REN1, HIGH); digitalWrite(REN2, HIGH);
      analogWrite(LPWM1, 0); analogWrite(LPWM2, 127);
      analogWrite(RPWM1, 127); analogWrite(RPWM2, 0);
      delay(actDelay);
    }

    // RIGHT SIDE TURN
    else if (!(distance3 <= maxRange || distance3 >= 0) && !(distance1 <= maxRange || distance1 >= 0) && !(distance2 <= maxRange || distance2 >= 0) && (distance4 <= maxRange || distance4 >= 0)) {
      digitalWrite(LEN1, HIGH); digitalWrite(LEN2, HIGH);
      digitalWrite(REN1, HIGH); digitalWrite(REN2, HIGH);
      analogWrite(LPWM1, 127); analogWrite(LPWM2, 0);
      analogWrite(RPWM1, 0); analogWrite(RPWM2, 127);
      delay(actDelay);
    }

    // SEARCH MODE
    else {
      digitalWrite(LEN1, HIGH); digitalWrite(LEN2, HIGH);
      digitalWrite(REN1, HIGH); digitalWrite(REN2, HIGH);
      analogWrite(LPWM1, 127); analogWrite(LPWM2, 127);
      analogWrite(RPWM1, 127); analogWrite(RPWM2, 127);
      delay(actDelay);
    }
  }
}
