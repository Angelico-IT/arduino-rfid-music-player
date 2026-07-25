int D1 = 10;
int D2 = 8;
int D3 = 7;
int A = 11;
int B = 12;
int C = 13;
int D = 14;
int E = 15;
int F = 16;
int G = 17;
const int ledPin1 = 6; // Simulate RED
const int ledPin2 = 5; // Simulate BLUE/GREEN
int buzzer = 4;

// Tetris theme melody notes and durations
int melody[] = {
  659, 494, 523, 587, 523, 494, 440, 
  440, 523, 659, 587, 523, 494, 523,
  587, 659, 523, 440, 440, 523, 587,
  659, 523, 440, 440, 523, 494, 523,
  587, 659, 784, 880, 698, 740, 784,
  523, 587, 659, 523, 440, 440
};

int durations[] = {
  250, 125, 250, 250, 250, 250, 500,
  250, 250, 500, 250, 250, 250, 250,
  250, 500, 250, 250, 250, 250, 250,
  500, 250, 250, 250, 250, 250, 250,
  250, 250, 250, 125, 250, 250, 250,
  250, 250, 500, 250, 250, 250
};

void setup() {
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  pinMode(E, OUTPUT);
  pinMode(F, OUTPUT);
  pinMode(G, OUTPUT);
}

void loop() {
  // === 3-2-1 Countdown ===
  showNumber(3);
  discoBlast(300);
  delay(300);

  showNumber(2);
  discoBlast(300);
  delay(300);

  showNumber(1);
  discoBlast(300);
  delay(300);

  // === Disco with Tetris Melody ===
  for (int i = 0; i < 40; i++) {
    // Display random segment pattern
    digitalWrite(D1, i % 3 == 0);
    digitalWrite(D2, i % 3 == 1);
    digitalWrite(D3, i % 3 == 2);

    digitalWrite(A, random(2));
    digitalWrite(B, random(2));
    digitalWrite(C, random(2));
    digitalWrite(D, random(2));
    digitalWrite(E, random(2));
    digitalWrite(F, random(2));
    digitalWrite(G, random(2));

    // LED strobe pattern
    digitalWrite(ledPin1, HIGH);
    digitalWrite(ledPin2, LOW);
    delay(50);
    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin2, HIGH);
    delay(50);

    // Play Tetris note
    int note = melody[i % (sizeof(melody)/sizeof(melody[0]))];
    int duration = durations[i % (sizeof(durations)/sizeof(durations[0]))];
    tone(buzzer, note, duration / 2);
    delay(duration);
  }

  delay(1000); // Pause before repeating
}

// === Helper Functions ===

void showNumber(int num) {
  digitalWrite(D1, HIGH); digitalWrite(D2, LOW); digitalWrite(D3, LOW);
  switch (num) {
    case 1:
      digitalWrite(A, LOW); digitalWrite(B, HIGH); digitalWrite(C, HIGH);
      digitalWrite(D, LOW); digitalWrite(E, LOW); digitalWrite(F, LOW); digitalWrite(G, LOW);
      break;
    case 2:
      digitalWrite(A, HIGH); digitalWrite(B, HIGH); digitalWrite(C, LOW);
      digitalWrite(D, HIGH); digitalWrite(E, HIGH); digitalWrite(F, LOW); digitalWrite(G, HIGH);
      break;
    case 3:
      digitalWrite(A, HIGH); digitalWrite(B, HIGH); digitalWrite(C, HIGH);
      digitalWrite(D, HIGH); digitalWrite(E, LOW); digitalWrite(F, LOW); digitalWrite(G, HIGH);
      break;
  }
}

void discoBlast(int duration) {
  digitalWrite(ledPin1, HIGH);
  digitalWrite(ledPin2, HIGH);
  tone(buzzer, 1500, duration / 2);
  delay(duration);
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);
  delay(duration / 2);
}
