/* macros and defines */
//L298P H-bridge
#define DE 5  //drive enable
#define DC 4  //drive control
#define SE 6  //steer enable
#define SC 7  //steer control

#define HLPIN 2 //headlight pin
#define BTSTATE 8 //bluetooth (HC-05) state
#define KRDATA 13 //pin 14 on the 75HC595
#define KRLATCH 3 //pin 12 on the 75HC595
#define KRCLK 12 //pin 10 on the 75HC595
#define LEDR 11 //LED pin red
#define LEDG 9 //LED pin green
#define LEDB 10 //LED pin blue

#define COMMON_ANODE

#define RED 255, 0, 0
#define BLUE 0, 0, 255
#define BLACK 0, 0, 0

#define FORWARD() dir.d = dir.f = 1
#define BACKWARD() dir.d = !(dir.f = 0)
#define LEFT() dir.s = dir.l = 1
#define RIGHT() dir.s = !(dir.l = 0)
#define HALT() dir.d = dir.s = 0

/* types */
enum {OFF, UP, DOWN};

typedef struct {
  int d; //drive
  int s; //steer
  int f; //forward
  int l; //left
} Direction;

/* global variables */
int hlstate = 0; //headlight state
int krstate = OFF; //knightrider state
unsigned int krpos = 1; //knightrider led position
unsigned long time = 0;
Direction dir;

/* function definitions */
void setcourse(int c) {
  if (c == 'F' || c == 'G' || c == 'I')
    FORWARD();
  else if (c == 'B' || c == 'H' || c == 'J')
    BACKWARD();
  else if (c == 'L' || c == 'R')
    dir.d = 0;
  if (c == 'G' || c == 'H' || c == 'L')
    LEFT();
  else if (c == 'I' || c == 'J' || c == 'R')
    RIGHT();
  else if (c == 'F' || c == 'B')
    dir.s = 0;
  if (c == 'S') //stop signal
    HALT();
}

void move() {
  digitalWrite(DE, dir.d);
  digitalWrite(DC, dir.f);
  digitalWrite(SE, dir.s);
  digitalWrite(SC, dir.l);
}

void setColor(int red, int green, int blue) {
  #ifdef COMMON_ANODE
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
  #endif
  //TODO it is always defined. Remove ifdef guards
  
  analogWrite(LEDR, red);
  analogWrite(LEDG, green);
  analogWrite(LEDB, blue);  
}
void knightrider() {
  char byte1, byte2;

  if (krpos >  (1 << 14))
    krstate = DOWN;
  else if (krpos < (1 << 1))
    krstate = UP;

  if (krstate == OFF) {
    byte1 = byte2 = 0;
    //TODO dont spam this
  } else {
    (krstate == UP) ? krpos <<= 1 : krpos >>= 1;
    byte1 = ((char*)&krpos)[0];
    byte2 = ((char*)&krpos)[1];
  }

  backlights(byte1, byte2);
}

void backlights(char b1, char b2) {
  digitalWrite(KRLATCH, LOW); //commence transmission
  shiftOut(KRDATA, KRCLK, MSBFIRST, b2); //dont reverse order, fix hardware
  shiftOut(KRDATA, KRCLK, MSBFIRST, b1);
  digitalWrite(KRLATCH, HIGH); //close transmission
}

void setup() {
  pinMode(DE, OUTPUT);
  pinMode(DC, OUTPUT);
  pinMode(SE, OUTPUT);
  pinMode(SC, OUTPUT);

  pinMode(HLPIN, OUTPUT);
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  pinMode(KRDATA, OUTPUT); //DS (Serial data input)
  pinMode(KRLATCH, OUTPUT); //STCP (Storage register clock input)
  pinMode(KRCLK, OUTPUT); //SHCP (Shift register clock input)

  pinMode(BTSTATE, INPUT);

  dir.d = dir.s = dir.f = dir.l = 0;

  Serial.begin(9600);
}

void loop() {
  if(digitalRead(BTSTATE) == LOW)
    HALT(); //connection lost
          
  int c = Serial.read();
  //TODO if old command, skip the if tests
  //TODO if no input, skip the if tests

  setcourse(c);
  move();

  if (c == 'W') //headlight
    digitalWrite(HLPIN, (hlstate = !hlstate));
  if (c == 'U') //backlight
    krstate = !krstate;

  if (millis() - time > 40) {
    knightrider();
    time = millis();
  }
}
