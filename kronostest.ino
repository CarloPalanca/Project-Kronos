// RIGHT DRIVER
#define RPWM2 5
#define LPWM2 6
#define REN2 7
#define LEN2 8
//LEFT DRIVER
#define RPWM1 9
#define LPWM1 10
#define REN1 11
#define LEN1 12
#define actDelay 10

void setup() {
  pinMode(LPWM1, OUTPUT);  pinMode(RPWM1, OUTPUT);
  pinMode(LPWM2, OUTPUT);  pinMode(RPWM2, OUTPUT);
  pinMode(LEN1, OUTPUT);   pinMode(LEN2, OUTPUT);
  pinMode(REN1, OUTPUT);   pinMode(REN2, OUTPUT);
}

void loop() {
  digitalWrite(LEN1, HIGH);
  digitalWrite(LEN2, HIGH);
  digitalWrite(REN1, HIGH);
  digitalWrite(REN2, HIGH);
  analogWrite(LPWM1, 127);
  analogWrite(LPWM2, 0);
  analogWrite(RPWM1, 0);
  analogWrite(RPWM2, 127);
  delay(actDelay);
}
