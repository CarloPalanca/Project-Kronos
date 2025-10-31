// Define pins
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
#define minRange 0
#define rampStep 10
#define rampDelay 20
#define chargePWM 255
#define trapPWM 80
#define spinPWM 110
#define detectionThreshold 5  // Number of detections to switch to aggressive mode
unsigned long lastActionTime = 0;
bool inCooldown = false;
int trig1 = 4;   int echo1 = 13;  // LEFT FRONT
int trig2 = A1;  int echo2 = A0;  // RIGHT FRONT
int trig3 = 2;   int echo3 = 3;   // LEFT SIDE
int trig4 = A3;  int echo4 = A2;  // RIGHT SIDE

// Aggression detection variables
int detectionCount = 0;
bool aggressive = false;

void setup() {
  pinMode(trig1, OUTPUT); pinMode(echo1, INPUT);
  pinMode(trig2, OUTPUT); pinMode(echo2, INPUT);
  pinMode(trig3, OUTPUT); pinMode(echo3, INPUT);
  pinMode(trig4, OUTPUT); pinMode(echo4, INPUT);
  pinMode(LPWM1, OUTPUT); pinMode(RPWM1, OUTPUT);
  pinMode(LPWM2, OUTPUT); pinMode(RPWM2, OUTPUT);
  pinMode(LEN1, OUTPUT);  pinMode(REN1, OUTPUT);
  pinMode(LEN2, OUTPUT);  pinMode(REN2, OUTPUT);
  pinMode(A4, INPUT); pinMode(A5, INPUT);
}

long getDistance(int trig, int echo) {
  digitalWrite(trig, LOW); delayMicroseconds(2);
  digitalWrite(trig, HIGH); delayMicroseconds(10); digitalWrite(trig, LOW);
  long duration = pulseIn(echo, HIGH);
  return (duration / 2) / 29.1;
}

void move(int lpwm1, int lpwm2, int rpwm1, int rpwm2) {
  digitalWrite(LEN1, HIGH); digitalWrite(LEN2, HIGH);
  digitalWrite(REN1, HIGH); digitalWrite(REN2, HIGH);
  analogWrite(LPWM1, lpwm1); analogWrite(LPWM2, lpwm2);
  analogWrite(RPWM1, rpwm1); analogWrite(RPWM2, rpwm2);
  delay(actDelay);
}

void rampAttack() {
  // Gradual speed increase for charging
  for (int speed = 127; speed <= 255; speed += rampStep) {
    move(speed, 0, speed, 0); // forward charging
    delay(rampDelay);
  }
}

void loop() {
  int AINL = digitalRead(A4); // IR LEFT
  int AINR = digitalRead(A5); // IR RIGHT

  long distance1 = getDistance(trig1, echo1);  // left front
  long distance2 = getDistance(trig2, echo2);  // right front
  long distance3 = getDistance(trig3, echo3);  // left side
  long distance4 = getDistance(trig4, echo4);  // right side

  bool opponentDetected = 
    (distance1 < maxRange || distance2 < maxRange || distance3 < maxRange || distance4 < maxRange);

  // Detect opponent
  if (opponentDetected) {
    detectionCount++;
  } else {
    detectionCount = 0;  // Reset if no opponent detected
  }

  // Aggressive mode after detecting the opponent multiple times
  if (detectionCount >= detectionThreshold) {
    aggressive = true;
  }

  // Edge avoidance
  if (!AINL && !AINR) { // Both sensors detect an edge
    move(0, 127, 0, 127); // reverse
    return;
  } else if (!AINL) { // Left sensor detects an edge
    move(0, 127, 0, 127); // reverse
    return;
  } else if (!AINR) { // Right sensor detects an edge
    move(0, 127, 0, 127); // reverse
    return;
  }

  // Attack mode based on opponent detection
  if (opponentDetected) {
    lastActionTime = millis();
    inCooldown = true;

    // Aggressive mode (full speed)
    if (aggressive) {
      move(chargePWM, 0, chargePWM, 0); // full-speed charge
    } else {
      rampAttack(); // Gradual ramping charge
    }

  } else {
    // Search mode (spin)
    move(spinPWM, 0, 0, spinPWM);
  }

  // Cooldown handling
  if (inCooldown && millis() - lastActionTime < 500) {
    move(0, 0, 0, 0); // pause for cooldown
    return;
  } else {
    inCooldown = false;
  }
}
