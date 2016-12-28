volatile unsigned long time;

void setup() {
  pinMode(13, OUTPUT);
  pinMode(3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(3), falling, FALLING);
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
}

void loop() {
  digitalWrite(13, digitalRead(3));
  
  if (!digitalRead(3)) {
    digitalWrite(10, LOW);
    time = millis();    
  } else if (millis() - time > 10 * 1000) {
    digitalWrite(10, HIGH);
  }
}

void falling() {
  digitalWrite(10, LOW);
  time = millis();
}

