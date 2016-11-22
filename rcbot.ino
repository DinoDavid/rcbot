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
char incomingByte;
int hlstate = 0; //headlight state
int hlpin = 2; //headlight pin
int nrstate = OFF; //nightrider state
unsigned int nrpos = 1; //Night Rider led position
int redPin = 11;
int greenPin = 9;
int bluePin = 10;
int dataPin = 13; //pin 14 on the 75HC595
int latchPin = 3; //pin 12 on the 75HC595
int clkPin = 12; //pin 10 on the 75HC595
const int BTState = 8; //bluetooth (HC-05) state

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
  
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);  
}
void knight_rider() {
  char byte1, byte2;

  if (nrpos >  (1 << 14)) //TODO check correct number
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
  delay(45);
}

void backlights(char b1, char b2) {
  digitalWrite(latchPin, LOW); //commence transmission
  shiftOut(dataPin, clkPin, MSBFIRST, b2); //send byte 1
  shiftOut(dataPin, clkPin, MSBFIRST, b1); //send byte 2
  digitalWrite(latchPin, HIGH); //close transmission
}

void setup() {
    pinMode(E1, OUTPUT);
    pinMode(E2, OUTPUT);

    pinMode(M1, OUTPUT);   
    pinMode(M2, OUTPUT); 

    pinMode(hlpin, OUTPUT); 
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT); 

    pinMode(dataPin, OUTPUT); //DS (Serial data input)
    pinMode(latchPin, OUTPUT); //STCP (Storage register clock input)
    pinMode(clkPin, OUTPUT); //SHCP (Shift register clock input)

    pinMode(BTState, INPUT);

    Serial.begin(9600);
}

void loop() {
  if(digitalRead(BTState) == LOW)
    halt(); //connection lost
          
  int c = Serial.read();

  if (c == 'F' || c == 'G' || c == 'I')
    forward();
  if (c == 'B' || c == 'H' || c == 'J')
    backward();
  if (c == 'G' || c == 'H' || c == 'L')
    left();
  if (c == 'I' || c == 'J' || c == 'R')
    right();

  if (c == 'W') //headlight
    hlstate ? digitalWrite(hlpin, (hlstate = LOW))
            : digitalWrite(hlpin, (hlstate = HIGH));

  if (c == 'U') //backlight
    nrstate = !nrstate;
  knight_rider();
    
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
      break; /* Fjern denne for a bruke spam-metoden. */
    default: /* Fanger release signal hvis ^ er ufjernet. */
      halt();
      setColor(BLACK);
  }
}
