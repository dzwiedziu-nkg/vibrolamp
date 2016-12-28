const unsigned long OFF_DELAY = 600000;
const unsigned long INPUT_DELAY = 1000;

volatile unsigned long timeOn = 0;
volatile unsigned long time = 0;
volatile unsigned int state = HIGH;


void setup() {
  pinMode(13, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(3), falling, FALLING);
  digitalWrite(10, state);
}

void loop() {
  digitalWrite(13, !digitalRead(3));

  unsigned long diff = millis() - timeOn;
  if (diff > OFF_DELAY) {
    state = HIGH;
    digitalWrite(10, state);
  }
}

void falling() {
  unsigned long diff = millis() - time;
  if (diff > INPUT_DELAY) {
    state = !state;
    digitalWrite(10, state);
    if (state == LOW) {
      timeOn = millis();
    }
  }
  time = millis();
}

