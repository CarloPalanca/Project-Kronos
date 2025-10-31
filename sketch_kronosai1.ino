// Motor Driver Pins
#define RPWM2 5
#define LPWM2 6
#define REN2 7
#define LEN2 8
#define RPWM1 9
#define LPWM1 10
#define REN1 11
#define LEN1 12

#define actDelay 10
#define maxRange 60

// Ultrasonic sensors
int trig1 = 4,   echo1 = 13;  // Front Left
int trig2 = A1,  echo2 = A0;  // Front Right
int trig3 = 2,   echo3 = 3;   // Left Side
int trig4 = A3,  echo4 = A2;  // Right Side

void setup() {
  pinMode(trig1, OUTPUT);  pinMode(echo1, INPUT);
  pinMode(trig2, OUTPUT);  pinMode(echo2, INPUT);
  pinMode(trig3, OUTPUT);  pinMode(echo3, INPUT);
  pinMode(trig4, OUTPUT);  pinMode(echo4, INPUT);

  pinMode(LPWM1, OUTPUT);  pinMode(RPWM1, OUTPUT);
  pinMode(LPWM2, OUTPUT);  pinMode(RPWM2, OUTPUT);
  pinMode(LEN1, OUTPUT);   pinMode(LEN2, OUTPUT);
  pinMode(REN1, OUTPUT);   pinMode(REN2, OUTPUT);

  pinMode(A4, INPUT);      pinMode(A5, INPUT); // IR sensors
}

long getDistance(int trig, int echo) {
  digitalWrite(trig, LOW); delayMicroseconds(2);
  digitalWrite(trig, HIGH); delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duration = pulseIn(echo, HIGH, 20000); // timeout added
  return (duration == 0) ? 999 : (duration / 2) / 29.1;
}

void loop() {
  int AINL = digitalRead(A4);
  int AINR = digitalRead(A5);

  long d1 = getDistance(trig1, echo1);
  long d2 = getDistance(trig2, echo2);
  long d3 = getDistance(trig3, echo3);
  long d4 = getDistance(trig4, echo4);

  // EDGE DETECTED - reverse and turn
  if (!AINL || !AINR) {
    digitalWrite(LEN1, HIGH); digitalWrite(LEN2, HIGH);
    digitalWrite(REN1, HIGH); digitalWrite(REN2, HIGH);

    if (!AINL && !AINR) {
      analogWrite(LPWM1, 0); analogWrite(RPWM1, 127);
      analogWrite(LPWM2, 0); analogWrite(RPWM2, 127);
    }
    else if (!AINL) {
      analogWrite(LPWM1, 0); analogWrite(RPWM1, 255);
      analogWrite(LPWM2, 127); analogWrite(RPWM2, 0);
    }
    else if (!AINR) {
      analogWrite(LPWM1, 127); analogWrite(RPWM1, 0);
      analogWrite(LPWM2, 0); analogWrite(RPWM2, 255);
    }
    delay(300); // Back off hard
    return;
  }

  // FIGHT MODE
  bool frontLeft = d1 < maxRange;
  bool frontRight = d2 < maxRange;
  bool sideLeft = d3 < maxRange;
  bool sideRight = d4 < maxRange;

  // --- ATTACK SEQUENCES ---
  if (frontLeft || frontRight) {
    int nearest = min(d1, d2);
    int startPWM = 127;
    int endPWM = (nearest <= 20) ? 255 : 200;
    int rampStep = 10;
    int delayStep = (nearest <= 20) ? 0 : (nearest <= 40 ? 15 : 30);

    for (int pwm = startPWM; pwm <= endPWM; pwm += rampStep) {
      digitalWrite(LEN1, HIGH); digitalWrite(LEN2, HIGH);
      digitalWrite(REN1, HIGH); digitalWrite(REN2, HIGH);
      analogWrite(LPWM1, pwm); analogWrite(LPWM2, 0);
      analogWrite(RPWM1, pwm); analogWrite(RPWM2, 0);
      delay(delayStep);
    }
    return;
  }

  // FLANK STRATEGY - turn to face opponent
  if (sideLeft) {
    digitalWrite(LEN1, HIGH); digitalWrite(LEN2, HIGH);
    digitalWrite(REN1, HIGH); digitalWrite(REN2, HIGH);
    analogWrite(LPWM1, 0); analogWrite(LPWM2, 127);
    analogWrite(RPWM1, 127); analogWrite(RPWM2, 0);
    delay(100);
    return;
  }
  if (sideRight) {
    digitalWrite(LEN1, HIGH); digitalWrite(LEN2, HIGH);
    digitalWrite(REN1, HIGH); digitalWrite(REN2, HIGH);
    analogWrite(LPWM1, 127); analogWrite(LPWM2, 0);
    analogWrite(RPWM1, 0); analogWrite(RPWM2, 127);
    delay(100);
    return;
  }

  // LOST TARGET - rotate to find opponent
  digitalWrite(LEN1, HIGH); digitalWrite(LEN2, HIGH);
  digitalWrite(REN1, HIGH); digitalWrite(REN2, HIGH);
  analogWrite(LPWM1, 127); analogWrite(LPWM2, 0);
  analogWrite(RPWM1, 0); analogWrite(RPWM2, 127);
  delay(100);
}
