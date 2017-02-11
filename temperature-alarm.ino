#include "pitches.h"
#include "got.h"

// Pins
const int temperatureSensorPin = A0;
const int resetButtonPin = 2;
const int soundPin = 3;

// Thresholds
const float temperatureThreshold = 20.0;
const long initialTimeThreshold = 600000; // 10 minutes
long timeThreshold = initialTimeThreshold;

// States
int buttonState = 0, lastButtonState = 0;
unsigned long startTime;
bool active = false;
float temperature;

// Configuration
const int timeThresholdMultiplier = 3;

void resetTimer() {
  startTime = millis();
  active = false;
}

void startTimer() {
  startTime = millis();
  active = true;
}

void printDebugInfo() {
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("\tStart Time: ");
  Serial.print(startTime);
  Serial.print("\tTimeThreshold: ");
  Serial.print(timeThreshold);
  Serial.print("\t Time Difference: ");
  Serial.println(millis() - startTime);
}

float readTemperature(int temperatureSensorPin) {
  int temperatureSensorValue = analogRead(temperatureSensorPin);
  float voltage = (temperatureSensorValue / 1024.0) * 5.0;
  return (voltage - .5) * 100;
}

void playAlarm(int soundPin) {
  int melody[] = {
    NOTE_E3, NOTE_A3, NOTE_C3, NOTE_D3, NOTE_E3, NOTE_A3, NOTE_C3, NOTE_D3
  };
  // note durations: 4 = quarter note, 8 = eighth note, etc.:
  int noteDurations[] = {
    4, 4, 8, 8, 4, 4, 8, 8
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
      tone(soundPin, NOTE_A4, 500); // confirmation
      timeThreshold =  timeThresholdMultiplier * initialTimeThreshold;
    }
  }
  lastButtonState = buttonState;

  temperature = readTemperature(temperatureSensorPin);
  printDebugInfo();
  if (temperature <= temperatureThreshold) {   // if temperature is below threshold
    if (active) {
      if (currentTime - startTime > timeThreshold) {
        playAlarm(soundPin);
      }
    } else { // put into active mode
      startTimer();
    }
  } else {
    resetTimer();
  }
  delay(500);
}

