//L298P H-bridge
#define DE 5  //drive enable
#define DC 4  //drive control
#include <Servo.h> 

Servo steer;

void setup() {
  pinMode(DE, OUTPUT);
  pinMode(DC, OUTPUT);
  steer.attach(9);
}

void loop() {

  digitalWrite(DE, HIGH);
  digitalWrite(DC, LOW);
  steer.write(150);

  delay(3000); 
  
  digitalWrite(DE, HIGH);
  digitalWrite(DC, HIGH);
  steer.write(50);

  delay(3000);
  
}


