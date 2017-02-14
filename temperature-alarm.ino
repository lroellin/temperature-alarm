#include "pitches.h"

// Pins
const int temperatureSensorPin = A0;
const int resetButtonPin = 2;
const int soundPin = 3;

// Thresholds
const float temperatureThreshold = 25.0;
const unsigned long initialTimeThreshold = 10000;

// Multiplier
const int timeThresholdMultiplier = 3;

// Variables
unsigned long timeThreshold, startTime, currentTime;
int buttonState, lastButtonState;
float temperature;
bool active;

// Time Functions
void resetTimer() {
  startTime = currentTime;
  active = false;
}

void startTimer() {
  startTime = millis();
  active = true;
}

unsigned long getPassedTime() {
  return currentTime - startTime;
}

// Temperature Functions
float readTemperature(int temperatureSensorPin) {
  int temperatureSensorValue = analogRead(temperatureSensorPin);
  float voltage = (temperatureSensorValue / 1024.0) * 5.0;
  return (voltage - .5) * 100;
}


bool hasTemperatureExceededThreshold() {
  return temperature >= temperatureThreshold;
}

// Sound Functions
// Note that this function introduces delays to play the melody correctly, so don't do any real-time stuff with it
void playAlarm() {
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

void printDebugInfo() {
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("\tDifference: ");
  Serial.println(temperature - temperatureThreshold);

  Serial.print("Current Time: ");
  Serial.print(currentTime);
  Serial.print("\tTimer Start Time: ");
  Serial.print(startTime);
  Serial.print("\tPassed Time Since: ");
  Serial.print(getPassedTime());
  Serial.print("\tThreshold:");
  Serial.println(timeThreshold);

  Serial.println("==================================================");
}

void setup() {
  // IO
  Serial.begin(9600);
  pinMode(temperatureSensorPin, INPUT);
  pinMode(resetButtonPin, INPUT);
  pinMode(soundPin, OUTPUT);
  // Initial values
  timeThreshold = initialTimeThreshold;
  startTime = millis();
  active = false;
}

void loop() {
  currentTime = millis();

  // Check if we need to reset
  buttonState = digitalRead(resetButtonPin);
  if (buttonState != lastButtonState && buttonState == HIGH) { // it was pressed now
    resetTimer();
    tone(soundPin, NOTE_A4, 500); // confirmation
    timeThreshold = timeThresholdMultiplier * initialTimeThreshold;
  }
  lastButtonState = buttonState;


  temperature = readTemperature(temperatureSensorPin);
  if (hasTemperatureExceededThreshold()) {
    if (!active) {
      startTimer();
    }
    if (getPassedTime() > timeThreshold) {
      playAlarm();
    }
  } else {
    resetTimer();
  }

  delay(500);
}


