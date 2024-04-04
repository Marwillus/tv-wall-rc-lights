#include <RCSwitch.h>
#include "EasingLib.h"

RCSwitch mySwitch = RCSwitch();
Easing slowEase(ease_mode::EASE_IN_OUT_CUBIC, 2000);
Easing fastEase(ease_mode::EASE_IN_OUT_CUBIC, 100);

int LED_PIN = 11;
int potiValue = 0;
bool mode = false;
bool modeChanged = mode;
int brightness = 0;
int smoothBrightness = 0;
int prevBrightness = 0;
float maxBrightness = 100;

unsigned long now;   // Store current millis().
unsigned long prev;  // store last measured millis().
unsigned long deltaT = 0;

void setup() {
  Serial.begin(9600);
  mySwitch.enableReceive(1);  // Receiver on interrupt 0 => that is pin #2
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  pingRcSwitch();
  pingPoti();

  if (mode == 1) {
    // fading up --> led1 for 2s, pause for 1s, led2 for 1s 
  }
  if (mode == 2) {
    //fading down --> all at same time
  }
  fadeTo();
}

void fadeTo() {
  if (brightness != prevBrightness) {
    smoothBrightness = fastEase.SetSetpoint(brightness);
    analogWrite(LED_PIN, smoothBrightness);
    Serial.println(smoothBrightness);
    prevBrightness = smoothBrightness;
  }
}

void pingRcSwitch() {
  if (mySwitch.available()) {
    Serial.print("Received Value: ");
    Serial.println(mySwitch.getReceivedValue());

    if (mySwitch.getReceivedValue() == 8685765 || mySwitch.getReceivedValue() == 8685767) {
      brightness = maxBrightness;
      // mode = true;
      Serial.println("on");
    }
    if (mySwitch.getReceivedValue() == 8685773 || mySwitch.getReceivedValue() == 8685775) {
      brightness = 0;
      // mode = false;
      Serial.println("off");
    }
    mySwitch.resetAvailable();
  }
}

void pingPoti() {
  // maxBrightness = map(analogRead(2), 5, 975, 0, 100);
}