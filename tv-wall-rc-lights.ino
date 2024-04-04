#include <RCSwitch.h>
#include "EasingLib.h"

RCSwitch mySwitch = RCSwitch();
Easing easing(ease_mode::EASE_IN_OUT_CUBIC, 1000);

int ledPin = 5;
int potiValue = 0;
bool mode = false;
bool modeChanged = mode;
int brightness = 0;
int smoothBrightness = 0;
int prevBrightness = 0;
float maxBrightness = 100;

unsigned long currentTime;          // Store current millis().
unsigned long previousTime;         // store last measured millis().
unsigned long pixelPrevious = 0;    // Previous Pixel Millis
unsigned long patternPrevious = 0;  // Previous Pattern Millis

void setup() {
  Serial.begin(9600);
  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
  pinMode(ledPin, OUTPUT);
}

void loop() {
  pingRcSwitch();

  // maxBrightness = map(analogRead(2), 0, 800, 0, 100);
  // maxBrightness = map(analogRead(2), 5, 975, 0, 100);

  // analogWrite(ledPin, maxBrightness);
  if (smoothBrightness != prevBrightness) {
    smoothBrightness = easing.SetSetpoint(brightness);
    analogWrite(ledPin, smoothBrightness);
    Serial.println(smoothBrightness);
    smoothBrightness = brightness;
  }
}

void pingRcSwitch() {
  if (mySwitch.available()) {
    Serial.print("Received Value: ");
    Serial.println(mySwitch.getReceivedValue());

    if (mySwitch.getReceivedValue() == 8685765 || mySwitch.getReceivedValue() == 8685767) {
      brightness = maxBrightness;
      Serial.println("on");
    }
    if (mySwitch.getReceivedValue() == 8685773 || mySwitch.getReceivedValue() == 8685775) {
      brightness = 0;
      Serial.println("off");
    }
    mySwitch.resetAvailable();
  }
}