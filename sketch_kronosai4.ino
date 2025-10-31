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
#define minRange 0
#define chargePWM 255
#define trapPWM 80
#define spinPWM 110
#define cooldownTime 500  // ms

// Ultrasonic sensors
int trig1 = 4;   int echo1 = 13;  // LEFT FRONT
int trig2 = A1;  int echo2 = A0;  // RIGHT FRONT
int trig3 = 2;   int echo3 = 3;   // LEFT SIDE
int trig4 = A3;  int echo4 = A2;  // RIGHT SIDE

// IR sensors
#define IR_LEFT A4
#define IR_RIGHT A5

// State tracking
unsigned long lastActionTime = 0;
bool inCooldown = false;

unsigned long lastSeenTime = 0;
unsigned long lastSpinChange = 0;
unsigned long spinChangeInterval = 1500;
const unsigned long minSpinInterval = 300;
const unsigned long intervalDecayTime = 5000;
const unsigned long spinDecayAmount = 200;

bool spinLeft = true;
bool spinPaused = false;
unsigned long spinPauseStart = 0;
const unsigned long spinPauseDuration = 300;

void setup() {
  pinMode(trig1, OUTPUT);  pinMode(echo1, INPUT);
  pinMode(trig2, OUTPUT);  pinMode(echo2, INPUT);
  pinMode(trig3, OUTPUT);  pinMode(echo3, INPUT);
  pinMode(trig4, OUTPUT);  pinMode(echo4, INPUT);

  pinMode(LPWM1, OUTPUT);  pinMode(RPWM1, OUTPUT);
  pinMode(LPWM2, OUTPUT);  pinMode(RPWM2, OUTPUT);
  pinMode(LEN1, OUTPUT);   pinMode(LEN2, OUTPUT);
  pinMode(REN1, OUTPUT);   pinMode(REN2, OUTPUT);

  pinMode(IR_LEFT, INPUT);
  pinMode(IR_RIGHT, INPUT);

  digitalWrite(LEN1, HIGH); digitalWrite(LEN2, HIGH);
  digitalWrite(REN1, HIGH); digitalWrite(REN2, HIGH);

  randomSeed(analogRead(0));  // Initialize randomness
}

long getDistance(int trig, int echo) {
  digitalWrite(trig, LOW); delayMicroseconds(2);
  digitalWrite(trig, HIGH); delayMicroseconds(10); digitalWrite(trig, LOW);
  long duration = pulseIn(echo, HIGH);
  return (duration / 2) / 29.1;
}

void stopMotors() {
  analogWrite(LPWM1, 0); analogWrite(LPWM2, 0);
  analogWrite(RPWM1, 0); analogWrite(RPWM2, 0);
}

void loop() {
  int AINL = digitalRead(IR_LEFT);
  int AINR = digitalRead(IR_RIGHT);

  long distance1 = getDistance(trig1, echo1);
  long distance2 = getDistance(trig2, echo2);
  long distance3 = getDistance(trig3, echo3);
  long distance4 = getDistance(trig4, echo4);

  bool opponentDetected = 
    (distance1 > minRange && distance1 < maxRange) ||
    (distance2 > minRange && distance2 < maxRange) ||
    (distance3 > minRange && distance3 < maxRange) ||
    (distance4 > minRange && distance4 < maxRange);

  // EDGE AVOIDANCE
  if (!AINL || !AINR) {
    analogWrite(LPWM1, 0);     analogWrite(LPWM2, 127);
    analogWrite(RPWM1, 0);     analogWrite(RPWM2, 127);
    delay(actDelay);
    return;
  }

  // COOLDOWN MODE
  if (inCooldown && millis() - lastActionTime < cooldownTime) {
    stopMotors();
    delay(actDelay);
    return;
  } else {
    inCooldown = false;
  }

  // OPPONENT DETECTED
  if (opponentDetected) {
    lastActionTime = millis();
    lastSeenTime = millis();
    spinChangeInterval = 1500;
    spinPaused = false;

    if (distance3 > minRange && distance3 < maxRange) {
      spinLeft = true;
    } else if (distance4 > minRange && distance4 < maxRange) {
      spinLeft = false;
    }

    if ((distance1 > 20 && distance1 < 40) || (distance2 > 20 && distance2 < 40)) {
      analogWrite(LPWM1, trapPWM); analogWrite(LPWM2, 0);
      analogWrite(RPWM1, trapPWM); analogWrite(RPWM2, 0);
    } else {
      analogWrite(LPWM1, chargePWM); analogWrite(LPWM2, 0);
      analogWrite(RPWM1, chargePWM); analogWrite(RPWM2, 0);
    }

    delay(actDelay);
    return;
  }

  // ADAPTIVE SPIN INTERVAL
  if (millis() - lastSeenTime > intervalDecayTime && spinChangeInterval > minSpinInterval) {
    spinChangeInterval -= spinDecayAmount;
    if (spinChangeInterval < minSpinInterval) spinChangeInterval = minSpinInterval;
    lastSeenTime = millis();  // reset decay timer
  }

  // SPIN PAUSE BEFORE CHANGING DIRECTION
  if (!spinPaused && millis() - lastSpinChange > spinChangeInterval) {
    spinPaused = true;
    spinPauseStart = millis();
    stopMotors();
  }

  if (spinPaused) {
    stopMotors();
    if (millis() - spinPauseStart > spinPauseDuration) {
      spinPaused = false;
      spinLeft = random(0, 2);
      lastSpinChange = millis();
    }
  } else {
    if (spinLeft) {
      analogWrite(LPWM1, spinPWM); analogWrite(LPWM2, 0);
      analogWrite(RPWM1, 0);        analogWrite(RPWM2, spinPWM);
    } else {
      analogWrite(LPWM1, 0);        analogWrite(LPWM2, spinPWM);
      analogWrite(RPWM1, spinPWM);  analogWrite(RPWM2, 0);
    }
  }

  delay(actDelay);
}
