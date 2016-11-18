#define E1 5  // Enable Pin for motor 1
#define E2 6  // Enable Pin for motor 2

#define M1 4  // Control pin 1 for motor 1
#define M2 7  // Control pin 2 for motor 1

void forward() {
  digitalWrite(E1, HIGH);
  digitalWrite(M1, LOW);
}

void backward() {
  digitalWrite(E1, HIGH);
  digitalWrite(M1, HIGH);
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

    Serial.begin(9600);
}

void loop() {
  int c = Serial.read();

  if (c == 'F' || c == 'G' || c == 'I')
    forward();
  if (c == 'B' || c == 'H' || c == 'J')
    backward();
  if (c == 'G' || c == 'H' || c == 'L')
    left();
  if (c == 'I' || c == 'J' || c == 'R')
    right();

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
  delay(10);
}
