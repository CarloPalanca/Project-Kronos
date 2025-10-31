// === MOTOR DRIVER PINS ===
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

// === SENSORS ===
#define IR_LEFT A4
#define IR_RIGHT A5

int trig1 = 4;   int echo1 = 13;  // LEFT FRONT
int trig2 = A1;  int echo2 = A0;  // RIGHT FRONT
int trig3 = 2;   int echo3 = 3;   // LEFT SIDE
int trig4 = A3;  int echo4 = A2;  // RIGHT SIDE

// === SETTINGS ===
#define actDelay 10
#define maxRange 60
#define rampStep 10
#define rampDelay 20
#define detectionThreshold 5

int detectionCount = 0;
bool aggressive = false;
String lastSeen = "none";

void setup() {
  pinMode(trig1, OUTPUT); pinMode(echo1, INPUT);
  pinMode(trig2, OUTPUT); pinMode(echo2, INPUT);
  pinMode(trig3, OUTPUT); pinMode(echo3, INPUT);
  pinMode(trig4, OUTPUT); pinMode(echo4, INPUT);

  pinMode(LPWM1, OUTPUT); pinMode(RPWM1, OUTPUT);
  pinMode(LPWM2, OUTPUT); pinMode(RPWM2, OUTPUT);
  pinMode(LEN1, OUTPUT);  pinMode(REN1, OUTPUT);
  pinMode(LEN2, OUTPUT);  pinMode(REN2, OUTPUT);

  pinMode(IR_LEFT, INPUT);
  pinMode(IR_RIGHT, INPUT);
}

long getDistance(int trig, int echo) {
  digitalWrite(trig, LOW); delayMicroseconds(2);
  digitalWrite(trig, HIGH); delayMicroseconds(10); digitalWrite(trig, LOW);
  long duration = pulseIn(echo, HIGH);
  return (duration / 2) / 29.1;
}

void move(int leftPWM, int rightPWM, bool leftFwd, bool rightFwd) {
  digitalWrite(LEN1, HIGH); digitalWrite(LEN2, HIGH);
  digitalWrite(REN1, HIGH); digitalWrite(REN2, HIGH);

  analogWrite(LPWM1, leftFwd ? 0 : leftPWM);
  analogWrite(RPWM1, leftFwd ? leftPWM : 0);
  analogWrite(LPWM2, rightFwd ? 0 : rightPWM);
  analogWrite(RPWM2, rightFwd ? rightPWM : 0);
  delay(actDelay);
}

void rampAttack() {
  for (int speed = 127; speed <= 255; speed += rampStep) {
    move(speed, speed, true, true);
    delay(rampDelay);
  }
}

void loop() {
  bool AINL = digitalRead(IR_LEFT);
  bool AINR = digitalRead(IR_RIGHT);

  long d1 = getDistance(trig1, echo1);  // front left
  long d2 = getDistance(trig2, echo2);  // front right
  long d3 = getDistance(trig3, echo3);  // side left
  long d4 = getDistance(trig4, echo4);  // side right

  // === EDGE AVOIDANCE ===
  if (!AINL && !AINR) {
    move(127, 127, false, false); // reverse
    delay(300);
    move(127, 50, true, true); // turn right
    delay(200);
    return;
  } else if (!AINL) {
    move(127, 127, false, false);
    delay(300);
    move(127, 50, true, true);
    delay(200);
    return;
  } else if (!AINR) {
    move(127, 127, false, false);
    delay(300);
    move(50, 127, true, true);
    delay(200);
    return;
  }

  // === OPPONENT DETECTION ===
  bool frontDetected = (d1 < maxRange || d2 < maxRange);
  bool sideLeft = d3 < maxRange;
  bool sideRight = d4 < maxRange;

  // === MEMORY + AGGRESSION MODE ===
  if (frontDetected || sideLeft || sideRight) {
    detectionCount++;
    if (d1 < maxRange) lastSeen = "left";
    if (d2 < maxRange) lastSeen = "right";
    if (d3 < maxRange) lastSeen = "left";
    if (d4 < maxRange) lastSeen = "right";
  } else {
    detectionCount = 0;
  }

  if (detectionCount >= detectionThreshold) aggressive = true;

  // === STRATEGY LOGIC ===
  if (frontDetected) {
    if (aggressive) {
      move(255, 255, true, true);
    } else {
      rampAttack();
    }
  } else if (sideLeft && !sideRight) {
    move(80, 127, true, true); // turn left
  } else if (sideRight && !sideLeft) {
    move(127, 80, true, true); // turn right
  } else {
    // SEARCH MODE
    if (lastSeen == "left") {
      move(80, 127, true, true); // turn left
    } else if (lastSeen == "right") {
      move(127, 80, true, true); // turn right
    } else {
      move(127, 127, true, false); // spin
    }
  }
}
