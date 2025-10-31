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
unsigned long lastAttackTime = 0;
unsigned long cooldownPeriod = 1000; // 1-second cooldown

// Define sensor pins
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
  int AINR = digitalRead(A5);  // analog input sensor C
  long duration1, distance1;
  long duration2, distance2;
  long duration4, distance4;
  long duration3, distance3;

  // Read sensors
  digitalWrite(trig1, LOW);   delayMicroseconds(2);      
  digitalWrite(trig1, HIGH);  delayMicroseconds(10);  digitalWrite(trig1, LOW);
  duration1 = pulseIn(echo1, HIGH);
  distance1 = (duration1 / 2) / 29.1;
  digitalWrite(trig2, LOW);   delayMicroseconds(2);    
  digitalWrite(trig2, HIGH);  delayMicroseconds(10);  digitalWrite(trig2, LOW);
  duration2 = pulseIn(echo2, HIGH);
  distance2 = (duration2 / 2) / 29.1;
  digitalWrite(trig3, LOW);   delayMicroseconds(2);      
  digitalWrite(trig3, HIGH);  delayMicroseconds(10);  digitalWrite(trig3, LOW);
  duration3 = pulseIn(echo3, HIGH);
  distance3 = (duration3 / 2) / 29.1;
  digitalWrite(trig4, LOW);   delayMicroseconds(2);      
  digitalWrite(trig4, HIGH);  delayMicroseconds(10);  digitalWrite(trig4, LOW);
  duration4 = pulseIn(echo4, HIGH);
  distance4 = (duration4 / 2) / 29.1;

  // Trap and Charge behavior
  if (AINL && AINR && millis() - lastAttackTime >= cooldownPeriod) {
    if ((distance1 > maxRange && distance2 > maxRange) && distance3 <= maxRange && distance4 > maxRange) {
      // Attack after trap
      chargeForward();
      lastAttackTime = millis();
    }
  }

  // Taunt spin behavior (optional)
  if (AINL && AINR && millis() - lastAttackTime >= cooldownPeriod && distance1 > maxRange && distance2 > maxRange) {
    tauntSpin();
    lastAttackTime = millis();
  }

  // Regular movement (default behavior when no other action is happening)
  if (!AINL && !AINR) { // 00
    moveBackward();
  } else if (!AINL && AINR) { // 01
    moveRight();
  } else if (AINL && !AINR) { // 10
    moveLeft();
  } else if (AINL && AINR) { // 11
    searchArea();
  }
}

void moveForward() {
  analogWrite(LPWM1, 127);  
  analogWrite(LPWM2, 127);  
  analogWrite(RPWM1, 127);  
  analogWrite(RPWM2, 127);  
  delay(actDelay);
}

void moveBackward() {
  analogWrite(LPWM1, 0);    
  analogWrite(LPWM2, 127);  
  analogWrite(RPWM1, 0);    
  analogWrite(RPWM2, 127);  
  delay(actDelay);
}

void moveLeft() {
  analogWrite(LPWM1, 0);    
  analogWrite(LPWM2, 127);  
  analogWrite(RPWM1, 127);  
  analogWrite(RPWM2, 0);    
  delay(actDelay);
}

void moveRight() {
  analogWrite(LPWM1, 127);  
  analogWrite(LPWM2, 0);    
  analogWrite(RPWM1, 0);    
  analogWrite(RPWM2, 127);  
  delay(actDelay);
}

void searchArea() {
  analogWrite(LPWM1, 127);  
  analogWrite(LPWM2, 127);  
  analogWrite(RPWM1, 127);  
  analogWrite(RPWM2, 127);  
  delay(actDelay);
}

void chargeForward() {
  analogWrite(LPWM1, 255);  
  analogWrite(LPWM2, 255);  
  analogWrite(RPWM1, 255);  
  analogWrite(RPWM2, 255);  
  delay(500);  // Charge for 500ms
}

void tauntSpin() {
  // Spin left for 500ms
  analogWrite(LPWM1, 127);
  analogWrite(LPWM2, 127);
  analogWrite(RPWM1, 127);
  analogWrite(RPWM2, 127);
  delay(500);
  
  // Spin right for 500ms
  analogWrite(LPWM1, 0);
  analogWrite(LPWM2, 0);
  analogWrite(RPWM1, 0);
  analogWrite(RPWM2, 0);
  delay(500);
}
