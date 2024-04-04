#include <RCSwitch.h>
#include "EasingLib.h"

RCSwitch mySwitch = RCSwitch();
Easing easing(ease_mode::EASE_IN_OUT_CUBIC, 1000);

int ledPin = 5;
bool mode = false;
bool modeChanged = mode;
int brightness = 100;
float maxBrightness = 0.4;

void setup() {
  Serial.begin(9600);
  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
  pinMode(ledPin, OUTPUT);
}

void loop() {
  if (mySwitch.available()) {

    Serial.print("Received Value: ");
    Serial.println(mySwitch.getReceivedValue());

    if (mySwitch.getReceivedValue() == 8685765) {
      brightness = 100 * maxBrightness/100;
      Serial.println("on");
    }
    if (mySwitch.getReceivedValue() == 8685773) {
      brightness = 0;
      Serial.println("off");
    }
    mySwitch.resetAvailable();
  }

  // maxBrightness = map(analogRead(2), 0, 800, 0, 100);
  maxBrightness = map(analogRead(2), 5, 975, 0, 100);
      Serial.println(easing.SetSetpoint(brightness));

  // analogWrite(ledPin, maxBrightness);

  // analogWrite(ledPin, easing.SetSetpoint(brightness));
}
