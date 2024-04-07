#include <RCSwitch.h>
#include "EasingLib.h"

RCSwitch mySwitch = RCSwitch();

const int FAST_DELAY = 2000;
const int SLOW_DELAY = 4000;
const int OFFSET = 800;

Easing fastEase(ease_mode::EASE_IN_OUT_CUBIC, FAST_DELAY);
Easing slowEase(ease_mode::EASE_IN_OUT_CUBIC, SLOW_DELAY);

int LED_1 = 10;
int LED_2 = 11;
int potiValue = 0;
int mode = 0;
bool nextMode = false;

int brightness = 0;
int brightness1 = 0;
int brightness2 = 0;
int smoothBrightness = 0;
int prevBrightness = 0;
float maxBrightness = 100;

unsigned long now;   // Store current millis().
unsigned long prev;  // store last measured millis().
unsigned long deltaT = 0;

void setup() {
  Serial.begin(9600);
  mySwitch.enableReceive(1);  // Receiver on interrupt 0 => that is pin #2
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
}

void loop() {
  now = millis();
  pingRcSwitch();
  pingPoti();

  switch (mode) {
    case 1:
      startTimer(2);
      break;
    case 2:
      // fading up --> led1 for 2s, pause for 1s, led2 for 1s
      fadeUp();
      break;
    case 3:
      //fading down --> all at same time
      fadeDown();
      break;

    default:
      break;
  }
}

void startTimer(int nextMode) {
  mode = nextMode;
  prev = now;
}

void fadeUp() {
  if (brightness1 <= maxBrightness) {
    // Serial.println(smoothBrightness);
    brightness1 = slowEase.SetSetpoint(maxBrightness);
    analogWrite(LED_1, brightness1);
  }
  if (now - prev >= OFFSET) {
    if (brightness2 < maxBrightness) {
      brightness2 = fastEase.SetSetpoint(maxBrightness);
      analogWrite(LED_2, brightness2);
    }
  }
  if (now - prev >= 4000) {
    blink();
    nextMode = true;
  }
  if (nextMode && brightness2 == maxBrightness) {
    nextMode = false;
    mode = 0;
  }
}

void fadeDown() {
  brightness1 = fastEase.SetSetpoint(0);
  brightness2 = slowEase.SetSetpoint(0);
  fastEase.SetSetpoint(0);
  analogWrite(LED_1, brightness1);
  analogWrite(LED_2, brightness2);

  if (smoothBrightness <= 0) {
    mode = 0;
  }
}

void blink() {
  analogWrite(LED_2, maxBrightness / 2);
  delay(200);
  analogWrite(LED_2, maxBrightness);
}

void pingRcSwitch() {
  if (mySwitch.available()) {
    Serial.print("Received Value: ");
    Serial.println(mySwitch.getReceivedValue());

    if (mySwitch.getReceivedValue() == 8685765 || mySwitch.getReceivedValue() == 8685767) {
      // brightness = maxBrightness;
      mode = 1;
      Serial.println("on");
    }
    if (mySwitch.getReceivedValue() == 8685773 || mySwitch.getReceivedValue() == 8685775) {
      // brightness = 0;
      mode = 3;
      Serial.println("off");
    }
    mySwitch.resetAvailable();
  }
}

void pingPoti() {
  // maxBrightness = map(analogRead(2), 5, 975, 0, 100);
}