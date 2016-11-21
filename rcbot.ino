/* macros and defines */
//L298P Connection  
#define E1 5  //enable motor 1
#define E2 6  //enable motor 2

#define M1 4  //control motor 1
#define M2 7  //control motor 2

#define COMMON_ANODE

#define RED (255, 0, 0)
#define BLUE (0, 0, 255)
#define BLACK (0, 0, 0)

/* global variables */
char incomingByte;
const int light  = 2;
int l = 0;
int redPin = 11;
int greenPin = 9;
int bluePin = 10;
int j = 0; //knight rider back lights
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

/*
void knight_rider() {
  int byte1, byte2;

  for (byte2 = 0; byte2 < 256; byte2++) {
    for (byte1 = 0; byte1 < 256; byte1++) {
        digitalWrite(latchPin, LOW); //pull latch LOW to start sending data
        shiftOut(dataPin, clkPin, MSBFIRST, byte1); //send the data byte 1
        shiftOut(dataPin, clkPin, MSBFIRST, byte2); //send the data byte 2
        digitalWrite(latchPin, HIGH); //pull latch HIGH to stop sending data
        delay(100);
    }
  }
}
*/

void setup() {
    pinMode(E1, OUTPUT);
    pinMode(E2, OUTPUT);

    pinMode(M1, OUTPUT);   
    pinMode(M2, OUTPUT); 

    pinMode(light, OUTPUT); 
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

//front lights
  if (c == 'W') {
    if (!l){
      digitalWrite(light, HIGH);
      l = 1;
    } else {
      digitalWrite(light, LOW); 
      l = 0;
    }
    c = 'w';
  }

//back lights
  if (c == 'U') {
    if (!j) {
      //knight_rider();
      j = 1;
    } else {
      digitalWrite(dataPin,LOW);     
      digitalWrite(latchPin,HIGH);     
      digitalWrite(clkPin,LOW); 
      j = 0;
    }
    c = 'u';
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
      break; /* Fjern denne for a bruke spam-metoden. */
    default: /* Fanger release signal hvis ^ er ufjernet. */
      halt();
      setColor(BLACK); 
  }
}
