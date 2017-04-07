unsigned long t1, t2;

enum {
  SERVO = 9, /* pin number of servo */
  PERIOD = 20000, /* micro seconds => 50hz */
  DEG_TO_DUTY = 16
};

void setup() {
  Serial.begin(9600);
  pinMode(7, INPUT);
  pinMode(SERVO, OUTPUT);
  t1 = 0; /* initialize timer */
}

void setAngle(unsigned long angle) {
  size_t duty_cycle;

  duty_cycle = angle*DEG_TO_DUTY;

  t2 = micros(); /* heed delay from other functions */
  
  digitalWrite(SERVO, HIGH);
  delayMicroseconds(duty_cycle);
  digitalWrite(SERVO, LOW);
  delayMicroseconds(PERIOD - duty_cycle - (t2-t1));
  
  t1 = micros();
}

char c = 0, a = 0;

/* 0-20 and 3-17 */
void loop() {
  if (Serial.available() > 0)
    c = Serial.read();
    
  //Serial.println("I received: " + c);
 
  if (c < '0' || '9' < c)
    a = 90;
  else 
    a = (c - '0') * 20;
    
  setAngle(a);
}
