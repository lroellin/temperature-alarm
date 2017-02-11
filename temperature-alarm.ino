#include "pitches.h"

// Pins
const int temperatureSensorPin = A0;
const int resetButtonPin = 2;
const int soundPin = 3;

// Thresholds
const float temperatureThreshold = 20.0;
long timeThreshold = 10000;

// States
int buttonState = 0, lastButtonState = 0;
unsigned long startTime;
bool timerSet = false;

// Configuration
const int timeThresholdMultiplier = 3;

void resetTimer() {
  startTime = millis();
  timerSet = false;
}

float readTemperature(int temperatureSensorPin) {
  int temperatureSensorValue = analogRead(temperatureSensorPin);
  float voltage = (temperatureSensorValue / 1024.0) * 5.0;
  return (voltage - .5) * 100;
}

void playAlarm(int soundPin) {
  int melody[] = {
    NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
  };

  // note durations: 4 = quarter note, 8 = eighth note, etc.:
  int noteDurations[] = {
    4, 8, 8, 4, 4, 4, 4, 4
  };

  for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(soundPin, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(soundPin);
  }
}

void setup() {
  pinMode(temperatureSensorPin, INPUT);
  pinMode(resetButtonPin, INPUT);
  pinMode(soundPin, OUTPUT);
  startTime = millis();
  Serial.begin(9600);
}

void loop() {
  unsigned long currentTime = millis();

  // Check if we need to reset time
  buttonState = digitalRead(resetButtonPin);
  if (buttonState != lastButtonState) { // has it changed
    if (buttonState == HIGH) { // is it pressed
      resetTimer();
      tone(soundPin, NOTE_A4, 500);
      timeThreshold *= timeThresholdMultiplier;
    }
  }
  lastButtonState = buttonState;

  float temperature = readTemperature(temperatureSensorPin);
  Serial.println(temperature);
  if (temperature <= temperatureThreshold) {   // if temperature is below threshold
    if (timerSet) {
      if (currentTime - startTime > timeThreshold) {
        playAlarm(soundPin);
      }
    } else {
      startTime = millis();
      timerSet = true;
    }
  } else {
    resetTimer();
  }
  delay(500);
}

