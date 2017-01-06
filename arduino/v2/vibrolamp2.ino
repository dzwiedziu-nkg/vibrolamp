/*
 * Vibrolamp v1.0
 * Copyright (C) by Michał Niedźwiecki 2017
 * License: GPLv3
 * 
 * Hardware:
 * - Arduino Pro Mini,
 * - Ball switch connected to PIN 3,
 * - Relay connected to PIN 10 (for lamp),
 * - Arduino integrated LED for notification
 * 
 * Features:
 * - Turn on relay (lamp) after rattling ball switch,
 * - Turn off relay after rattling again,
 * - Turn off relay after 10 minutes from turn on
 * 
 * Starting:
 * 1. Initialize
 * 2. Testing procedure (around 3.5s)
 * 3. Operational loop (ready to use)
 * 
 * Used technologies:
 * - interrupt (on falling),
 * - reading time from µC starting,
 * - delay,
 * - volatile variables,
 * - input and output pins,
 * - quick multiple switching protection
 */

#define POWER_ON    LOW
#define POWER_OFF   HIGH
#define NOTIFY_LED  13                    // pin for notification LED
#define RELAY_PIN   10                    // pin for control relay (lamp)
#define BALL_PIN    3                     // pin for ball switch input

const unsigned long OFF_DELAY = 600000;   // turn off relay after 10 minutes
const unsigned long INPUT_DELAY = 500;    // lock switch for 500ms since latest ball switch event

volatile unsigned long timeOn = 0;        // timestamp of latest turn on event
volatile unsigned long timeLock = 0;      // timestamp of latest ball switch event (rattling)
volatile unsigned int state = POWER_OFF;  // current state of lamp (relay)
volatile unsigned int lock = false;       // ball switch event lock


void setup() {
  pinMode(NOTIFY_LED, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BALL_PIN, INPUT_PULLUP);

  testingProcedure();

  // setup interrupt procedure for ball switch event
  attachInterrupt(digitalPinToInterrupt(BALL_PIN), falling, FALLING);
  
  digitalWrite(RELAY_PIN, state);
}

void loop() {
  notify();
  unsigned long millisValue = millis();

  // release ball switch event lock after INPUT_DELAY since latest event
  if (lock) {
    long diff = millisValue - timeLock;
    if (abs(diff) > INPUT_DELAY) {
      lock = false;
    }
  }

  // turn off after OFF_DELAY since turn on
  if (state == POWER_ON) {
    long diff = millisValue - timeOn;
    if (abs(diff) > OFF_DELAY) {
      state = POWER_OFF;
    }
  }
  
  digitalWrite(RELAY_PIN, state);
}

/*
 * Ball switch event procedure (interrupt procedure).
 * 1. Switch (turn on or turn off) when no locked
 * 2. Lock after switch
 * 3. Save timeOn if turn on
 * 4. Save timeLock always
 */
void falling() {
  if (!lock) {
    state = !state;
    lock = true;
    if (state == POWER_ON) {
      timeOn = millis();
    }
  }
  timeLock = millis();
}

/*
 * Testing procedure (long: 3.5s):
 * 1. Turn off and delay 500ms
 * 2. Turn on and delay 1s
 * 3. If balls connected then turn off
 * 4. Delay 1s
 * 5. Turn on and delay 1s
 */
void testingProcedure() {
  digitalWrite(RELAY_PIN, POWER_OFF);
  digitalWrite(NOTIFY_LED, LOW);
  delay(500);
  digitalWrite(RELAY_PIN, POWER_ON);
  digitalWrite(NOTIFY_LED, HIGH);
  delay(1000);
  digitalWrite(RELAY_PIN, digitalRead(BALL_PIN));
  digitalWrite(NOTIFY_LED, !digitalRead(BALL_PIN));
  delay(1000);
  digitalWrite(RELAY_PIN, POWER_ON);
  digitalWrite(NOTIFY_LED, HIGH);
  delay(1000);
}

/*
 * Notify about ball switch state and lock state:
 * 1. No locked,  balls connected:     dark
 * 2. No locked,  balls disconnected:  light
 * 3. Locked,     balls connected:     light
 * 4. Locked,     balls disconnected:  dark
 */
void notify() {
  int pinValue = digitalRead(BALL_PIN);
  if (lock) {
    digitalWrite(NOTIFY_LED, pinValue);    
  } else {
    digitalWrite(NOTIFY_LED, !pinValue);    
  }
}

