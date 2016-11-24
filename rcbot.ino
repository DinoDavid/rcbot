/* macros and defines */
//L298P Connection  
#define E1 5  //enable motor 1
#define E2 6  //enable motor 2
#define M1 4  //control motor 1
#define M2 7  //control motor 2

#define HLPIN 2 //headlight pin
#define BTSTATE 8 //bluetooth (HC-05) state
#define NRDATA 13 //pin 14 on the 75HC595
#define NRLATCH 3 //pin 12 on the 75HC595
#define NRCLK 12 //pin 10 on the 75HC595
#define LEDR 11 //LED pin red
#define LEDG 9 //LED pin green
#define LEDB 10 //LED pin blue

#define COMMON_ANODE

#define RED 255, 0, 0
#define BLUE 0, 0, 255
#define BLACK 0, 0, 0

/* types */
enum {OFF, UP, DOWN};

/* global variables */
int hlstate = 0; //headlight state
int nrstate = OFF; //nightrider state
unsigned int nrpos = 1; //nightrider led position
unsigned long time = 0;

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
  setColor(BLACK);
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
  
  analogWrite(LEDR, red);
  analogWrite(LEDG, green);
  analogWrite(LEDB, blue);  
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
  digitalWrite(NRLATCH, LOW); //commence transmission
  shiftOut(NRDATA, NRCLK, MSBFIRST, b2); //dont reverse order, fix hardware
  shiftOut(NRDATA, NRCLK, MSBFIRST, b1);
  digitalWrite(NRLATCH, HIGH); //close transmission
}

void setup() {
    pinMode(E1, OUTPUT);
    pinMode(E2, OUTPUT);

    pinMode(M1, OUTPUT);   
    pinMode(M2, OUTPUT); 

    pinMode(HLPIN, OUTPUT); 
    pinMode(LEDR, OUTPUT);
    pinMode(LEDG, OUTPUT);
    pinMode(LEDB, OUTPUT); 

    pinMode(NRDATA, OUTPUT); //DS (Serial data input)
    pinMode(NRLATCH, OUTPUT); //STCP (Storage register clock input)
    pinMode(NRCLK, OUTPUT); //SHCP (Shift register clock input)

    pinMode(BTSTATE, INPUT);

    Serial.begin(9600);
}

void loop() {
  if(digitalRead(BTSTATE) == LOW)
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
  if (c == 'S') //stop signal
      halt();
  if (c == 'W') //headlight
    digitalWrite(HLPIN, (hlstate = !hlstate));
  if (c == 'U') //backlight
    nrstate = !nrstate;

  if (millis() - time > 40) {
    knight_rider();
    time = millis();
  }
}
