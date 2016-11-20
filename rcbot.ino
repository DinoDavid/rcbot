char incomingByte;

//L298P Connection  
#define E1 5  // Enable Pin for motor 1
#define E2 6  // Enable Pin for motor 2

#define M1 4  // Control pin 1 for motor 1
#define M2 7  // Control pin 2 for motor 2

//Leds connected to Arduino UNO Pin 9
const int light  = 2;
int l = 0;

//RGB LED
int redPin = 11;
int greenPin = 9;
int bluePin = 10;
#define COMMON_ANODE

//Knight Rider back lights
int j = 0;
int dataPin = 13;   //pin 14 on the 75HC595
int latchPin = 3;  //pin 12 on the 75HC595
int clkPin = 12; //pin 10 on the 75HC595

int byte1 = 0;         
int byte2 = 0;

//Bluetooth (HC-05) State pin connected to Arduino UNO Pin 8
const int BTState = 8;
  
void forward() {
  digitalWrite(E1, HIGH);
  digitalWrite(M1, HIGH);
  blue();
}

void backward() {
  digitalWrite(E1, HIGH);
  digitalWrite(M1, LOW);
  red();
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

void setColor(int red, int green, int blue)
{   
  #ifdef COMMON_ANODE
  red = 255 - red;
  green = 255 - green;
  blue = 255 - blue;
  #endif
  
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);  
}
void yellow(){
  setColor(255, 255, 0);
}

void pink(){
  setColor(255, 0, 255);
}

void aqua(){
  setColor(0, 255, 255);
}

void red(){
  setColor(255, 0, 0); 
}

void green(){
  setColor(0, 255, 0); 
}

void blue(){
  setColor(0, 0, 255); 
}

void white(){
  setColor(255, 255, 255); 
}

void black(){
  setColor(0, 0, 0); 
}

/*
void knight_rider(){
  for (byte2 = 0; byte2 < 256; byte2++)               //Outer Loop
    {
        for (byte1 = 0; byte1 < 256; byte1++)            //Inner Loop
        {
        digitalWrite(latchPin, LOW);            //Pull latch LOW to start sending data
        shiftOut(dataPin, clkPin, MSBFIRST, byte1); //Send the data byte 1
        shiftOut(dataPin, clkPin, MSBFIRST, byte2); //Send the data byte 2
        digitalWrite(latchPin, HIGH);           //Pull latch HIGH to stop sending data
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
//Stop car when connection lost or bluetooth disconnected
  if(digitalRead(BTState)==LOW)
    halt();
          
  int c = Serial.read();

  if (c == 'F' || c == 'G' || c == 'I')
    forward();
  if (c == 'B' || c == 'H' || c == 'J')
    backward();
  if (c == 'G' || c == 'H' || c == 'L')
    left();
  if (c == 'I' || c == 'J' || c == 'R')
    right();

//Front lights
  if (c == 'W'){
    if (l == 0){
      digitalWrite(light, HIGH);
      l=1;
    }
    else if (l == 1){
      digitalWrite(light, LOW); 
      l=0;
    }
    c='w';
  }

//Back lights
   if (c == 'U'){
    if (j == 0){
      //knight_rider();
      j=1;
    }
    else if (j == 1){
      digitalWrite(dataPin,LOW);     
      digitalWrite(latchPin,HIGH);     
      digitalWrite(clkPin,LOW); 
      j=0;
    }
    c='u';
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
      black();    
  }
}
