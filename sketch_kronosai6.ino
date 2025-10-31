#define RPWM2 5
#define LPWM2 6
#define REN2 7
#define LEN2 8
#define RPWM1 9
#define LPWM1 10
#define REN1 11
#define LEN1 12
#define moveSpeed 127
#define maxRange  60                   // Adjustable - 60 for ASICUP
int trig1 = 4,    echo1 = 13;
int trig2 = A1,   echo2 = A0;
int trig3 = 2,    echo3 = 3;
int trig4 = A3,   echo4 = A2;
int AINL = A4;
int AINR = A5;
int spd = moveSpeed * 0.6;
bool holding = false;
unsigned long holdStartTime = 0;
int holdStage = 0;
void setup() {
  pinMode(trig1, OUTPUT); pinMode(echo1, INPUT);
  pinMode(trig2, OUTPUT); pinMode(echo2, INPUT);
  pinMode(trig3, OUTPUT); pinMode(echo3, INPUT);
  pinMode(trig4, OUTPUT); pinMode(echo4, INPUT);
  pinMode(LPWM1, OUTPUT); pinMode(RPWM1, OUTPUT);
  pinMode(LPWM2, OUTPUT); pinMode(RPWM2, OUTPUT);
  pinMode(LEN1, OUTPUT);  pinMode(LEN2, OUTPUT);
  pinMode(REN1, OUTPUT);  pinMode(REN2, OUTPUT);
  pinMode(AINL, INPUT);   pinMode(AINR, INPUT);
} long getDistance(int trig, int echo) {
  digitalWrite(trig, LOW); delayMicroseconds(2);
  digitalWrite(trig, HIGH); delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duration = pulseIn(echo, HIGH);
  return (duration == 0) ? 999 : (duration / 2) / 29.1;
} void loop() {
  long d1 = getDistance(trig1, echo1); long d3 = getDistance(trig3, echo3);
  long d2 = getDistance(trig2, echo2); long d4 = getDistance(trig4, echo4);
  digitalWrite(LEN1, HIGH); digitalWrite(LEN2, HIGH);
  digitalWrite(REN1, HIGH); digitalWrite(REN2, HIGH);
  if (holding) {
    unsigned long currentMillis = millis();
    if (holdStage == 0 && currentMillis - holdStartTime >= 1000) {
      analogWrite(LPWM1, moveSpeed); analogWrite(RPWM1, 0);
      analogWrite(LPWM2, moveSpeed); analogWrite(RPWM2, 0);
      holdStage = 1;
      holdStartTime = currentMillis;
    } else if (holdStage == 1 && currentMillis - holdStartTime >= 100) {
      holding = false;
      holdStage = 0;
    }
    return;
  } if (!digitalRead(AINL) || !digitalRead(AINR)) {
    reverseAndTurn();
    return;
  } int attackSpeed = calculateAttackSpeed(d1, d2);
  if (d1 < maxRange || d4 < maxRange) {
    int nearest = min(d1, d2);
    adjustAttackSpeed(nearest, attackSpeed);
    return;
  } if (d3 < maxRange || d2 < maxRange) {
    if (d3 < d2) {
      turnLeft();
    } else {
      turnRight();
    }
    return;
  }
  holding = true;
  holdStartTime = millis();
  holdStage = 0;
  analogWrite(LPWM1, moveSpeed); analogWrite(RPWM1, moveSpeed);
  analogWrite(LPWM2, moveSpeed); analogWrite(RPWM2, moveSpeed);
} int calculateAttackSpeed(long dist1, long dist2) {
  int nearest = min(dist1, dist2);
  int speed;
  if (nearest <= 10) {
    speed = moveSpeed;
  } else if (nearest <= 40) {
    speed = map(nearest, 20, 40, moveSpeed * 0.9, moveSpeed * 0.8);
  } else {
    speed = 127 * 0.7;
  }
  return speed;
} void adjustAttackSpeed(int nearest, int attackSpeed) {
  int rampStep = 10;
  int rampDelay;
  if (nearest > 40) {
    rampDelay = 30;
  } else if (nearest > 20) {
    rampDelay = 15;
  } else {
    rampDelay = 0;
  } for (spd; spd <= attackSpeed; spd += rampStep) {
    analogWrite(LPWM1, spd); analogWrite(RPWM1, 0);
    analogWrite(LPWM2, spd); analogWrite(RPWM2, 0);
    if (rampDelay > 0) delay(rampDelay);
  }
} void hold() {
  analogWrite(LPWM1, moveSpeed); analogWrite(RPWM1, moveSpeed);
  analogWrite(LPWM2, moveSpeed); analogWrite(RPWM2, moveSpeed);
} void turnLeft() {
  analogWrite(LPWM1, 0); analogWrite(RPWM1, spd);
  analogWrite(LPWM2, spd); analogWrite(RPWM2, 0);
} void turnRight() {
  analogWrite(LPWM1, spd); analogWrite(RPWM1, 0);
  analogWrite(LPWM2, 0); analogWrite(RPWM2, spd);
} void reverseAndTurn() {
  analogWrite(LPWM1, 0); analogWrite(RPWM1, moveSpeed);
  analogWrite(LPWM2, 0); analogWrite(RPWM2, moveSpeed);
  delay(600);
}
