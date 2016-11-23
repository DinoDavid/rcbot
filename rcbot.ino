/* macros and defines */
//L298P Connection  
#define E1 5  //enable motor 1
#define E2 6  //enable motor 2

#define M1 4  //control motor 1
#define M2 7  //control motor 2

#define COMMON_ANODE

#define RED 255, 0, 0
#define BLUE 0, 0, 255
#define BLACK 0, 0, 0

/* types */
enum {OFF, UP, DOWN};

/* global variables */
unsigned long time = 0;
int btstate = 8; //bluetooth (HC-05) state
int hlstate = 0; //headlight state
int hlpin = 2; //headlight pin
int nrstate = OFF; //nightrider state
int nrdata = 13; //pin 14 on the 75HC595
int nrlatch = 3; //pin 12 on the 75HC595
int nrclk = 12; //pin 10 on the 75HC595
unsigned int nrpos = 1; //nightrider led position
int ledr = 11; //LED pin red
int ledg = 9; //LED pin green
int ledb = 10; //LED pin blue

/* function definitions */
void forward() {
  digitalWrite(E1, HIGH);
  digitalWrite(M1, HIGH);
  setColor(BLUE); 
}

void backward() {
  digitalWrite(E1, HIGH);
  digitalWrite(M1, LOW);
  setColor(RED); 
}

void halt() {
  analogWrite(E1, 0);
  analogWrite(E2, 0);  
}

void left() {
  digitalWrite(E2, HIGH);
  digitalWrite(M2, HIGH);
}

void right() {
  digitalWrite(E2, HIGH);
  digitalWrite(M2, LOW);
}

void setColor(int red, int green, int blue) {
  #ifdef COMMON_ANODE
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
  #endif
  //TODO it is always defined. Remove ifdef guards
  
  analogWrite(ledr, red);
  analogWrite(ledg, green);
  analogWrite(ledb, blue);  
}
void knight_rider() {
  char byte1, byte2;

  if (nrpos >  (1 << 14))
    nrstate = DOWN;
  else if (nrpos < (1 << 1))
    nrstate = UP;

  if (nrstate == OFF) {
    byte1 = byte2 = 0;
    //TODO dont spam this
  } else {
    (nrstate == UP) ? nrpos <<= 1 : nrpos >>= 1;
    byte1 = ((char*)&nrpos)[0];
    byte2 = ((char*)&nrpos)[1];
  }

  backlights(byte1, byte2);
}

void backlights(char b1, char b2) {
  digitalWrite(nrlatch, LOW); //commence transmission
  shiftOut(nrdata, nrclk, MSBFIRST, b2); //dont reverse order, fix hardware
  shiftOut(nrdata, nrclk, MSBFIRST, b1);
  digitalWrite(nrlatch, HIGH); //close transmission
}

void setup() {
    pinMode(E1, OUTPUT);
    pinMode(E2, OUTPUT);

    pinMode(M1, OUTPUT);   
    pinMode(M2, OUTPUT); 

    pinMode(hlpin, OUTPUT); 
    pinMode(ledr, OUTPUT);
    pinMode(ledg, OUTPUT);
    pinMode(ledb, OUTPUT); 

    pinMode(nrdata, OUTPUT); //DS (Serial data input)
    pinMode(nrlatch, OUTPUT); //STCP (Storage register clock input)
    pinMode(nrclk, OUTPUT); //SHCP (Shift register clock input)

    pinMode(btstate, INPUT);

    Serial.begin(9600);
}

void loop() {
  if(digitalRead(btstate) == LOW)
    halt(); //connection lost
          
  int c = Serial.read();
  //TODO if old command, skip the if tests
  //TODO if no input, skip the if tests

  if (c == 'F' || c == 'G' || c == 'I')
    forward();
  if (c == 'B' || c == 'H' || c == 'J')
    backward();
  if (c == 'G' || c == 'H' || c == 'L')
    left();
  if (c == 'I' || c == 'J' || c == 'R')
    right();

  if (c == 'W') //headlight
    digitalWrite(hlpin, (hlstate = !hlstate));

  if (c == 'U') //backlight
    nrstate = !nrstate;
  if (millis() - time > 40) {
    knight_rider();
    time = millis();
  }
    
  switch(c) {
    case 'F':
    case 'B':
    case 'L':
    case 'R':
    case 'G':
    case 'I':
    case 'H':
    case 'J':
    case -1:
      break; //remove to use spam method
    default: //catch release signal
      halt(); //TODO catch stop signal explicitly
      setColor(BLACK);
  }
}
