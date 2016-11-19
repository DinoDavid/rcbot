char incomingByte;

//L298P Connection  
#define E1 5  // Enable Pin for motor 1
#define E2 6  // Enable Pin for motor 2

#define M1 4  // Control pin 1 for motor 1
#define M2 7  // Control pin 2 for motor 2

//Leds connected to Arduino UNO Pin 9
const int light  = 9;
int l = 0;

//Bluetooth (HC-05) State pin connected to Arduino UNO Pin 8
const int BTState = 8;
  
void forward() {
  digitalWrite(E1, HIGH);
  digitalWrite(M1, HIGH);
}

void backward() {
  digitalWrite(E1, HIGH);
  digitalWrite(M1, LOW);
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

void setup() {
    pinMode(E1, OUTPUT);
    pinMode(E2, OUTPUT);

    pinMode(M1, OUTPUT);   
    pinMode(M2, OUTPUT); 

    pinMode(light, OUTPUT); 
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
  
//Light    
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
  }
}
