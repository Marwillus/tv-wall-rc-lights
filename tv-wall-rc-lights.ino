#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

int LED_1 = 9;
int LED_2 = 10;
int POTI_PIN = 2;

bool powerOn = false;
int mode = 0;

int potiValue = 0;
int oldPotiValue = 0;
int minPotiValue = 5;
int maxPotiValue = 950;

int brightness1 = 0;
int brightness2 = 0;
int maxBrightness = 100;

unsigned long now;
long prevDelay;
long prevFadeLed1 = 0;
long prevFadeLed2 = 0;
float t1Up = 0.0;
float t2Up = 0.0;
float t1Down = 1.0;
bool fadeComplete1 = true;
bool fadeComplete2 = true;
int fadeInterval = 20;


void setup() {
  // Serial.begin(9600);
  mySwitch.enableReceive(1);  // Receiver on interrupt 0 => that is pin #2
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
}

void loop() {
  now = millis();
  pingRcSwitch();
  if (powerOn) pingPoti();

  switch (mode) {
    case 0:
      if (powerOn) {
        analogWrite(LED_1, maxBrightness);
        analogWrite(LED_2, maxBrightness / 2);
      }
      break;
    case 1:
      fadeUpAnimation();
      break;
    case 2:
      fadeDown();
      break;

    default:
      break;
  }
}

void startDelayTimer() {
  prevDelay = now;
}

void fadeUpAnimation() {
  fadeUpOnce(1, 2000);
  // offset delay
  if (now - prevDelay >= 1000) {
    fadeUpOnce(2, 6000);
  }
  // if (now - prevDelay >= 4000) {
  //   blink();
  // }
  if (fadeComplete1 && fadeComplete2) {
    mode = 0;
  }
}

void fadeUpOnce(int led, int duration) {
  if (!fadeComplete1 && led == 1) {
    if (now - prevFadeLed1 >= fadeInterval) {
      prevFadeLed1 = now;

      // Serial.print("brightness1: ");
      // Serial.println(brightness1);

      if (t1Up <= 1.0) {
        brightness1 = (int)(maxBrightness * easeOutBack(t1Up));
        analogWrite(LED_1, brightness1);
        t1Up += (float)fadeInterval / duration;
      } else {
        t1Up = 0.0;
        fadeComplete1 = true;
      }
    }
  }
  if (!fadeComplete2 && led == 2) {
    if (now - prevFadeLed2 >= fadeInterval) {
      prevFadeLed2 = now;

      // Serial.print("Fadeup 2: ");
      // Serial.print(t2Up);
      // Serial.print(", ");
      // Serial.print("brightness2: ");
      // Serial.println(brightness2);

      if (t2Up <= 1.0) {
        brightness2 = (int)(maxBrightness * easeOutQuad(t2Up));
        analogWrite(LED_2, brightness2 / 2);
        t2Up += (float)fadeInterval / duration;
      } else {
        t2Up = 0.0;
        fadeComplete2 = true;
      }
    }
  }
}

void fadeDown() {
  if (!fadeComplete1 && now - prevFadeLed1 >= fadeInterval) {
    prevFadeLed1 = now;
    const float duration = 1500.0;

    if (t1Down >= 0.0) {
      brightness1 = (int)(maxBrightness * easeInOutQuad(t1Down));
      brightness2 = (int)(maxBrightness * easeOutQuad(t1Down));
      analogWrite(LED_1, brightness1);
      analogWrite(LED_2, brightness2 / 2);
      t1Down -= fadeInterval / duration;

      // Serial.print("Fade down: ");
      // Serial.println(brightness1);
    }

    if (t1Down < 0.0) {
      t1Down = 1.0;
      fadeComplete1 = true;
      fadeComplete2 = true;
      mode = 0;
    }
  }
}

void blink() {
  analogWrite(LED_2, maxBrightness / 2);
  delay(200);
  analogWrite(LED_2, maxBrightness);
}

void pingRcSwitch() {
  if (mySwitch.available()) {
    if (mySwitch.getReceivedValue() == 8685765 || mySwitch.getReceivedValue() == 8685767) {
      if (fadeComplete1 && fadeComplete2) {
        startDelayTimer();
        resetFade();
        powerOn = true;
        mode = 1;
        // Serial.println("on");
      }
    }
    if (mySwitch.getReceivedValue() == 8685773 || mySwitch.getReceivedValue() == 8685775) {
      if (powerOn) {
        startDelayTimer();
        resetFade();
        powerOn = false;
        mode = 2;
        // Serial.println("off");
      }
    }
    mySwitch.resetAvailable();
  }
}

void pingPoti() {
  potiValue = analogRead(POTI_PIN);

  //adjust only if poti value changes
  if (oldPotiValue > potiValue + 4 || oldPotiValue < potiValue - 4) {

    // adjust max min value of poti
    if (potiValue < minPotiValue) {
      minPotiValue = potiValue;
    }
    if (potiValue > maxPotiValue) {
      maxPotiValue = potiValue;
    }

    maxBrightness = map(potiValue, minPotiValue, maxPotiValue, 18, 255);

    oldPotiValue = potiValue;
  }
}

void resetFade() {
  fadeComplete1 = false;
  fadeComplete2 = false;
  t1Up = 0.0;
  t2Up = 0.0;
  t1Down = 1.0;
}

float easeOutBack(float x) {
  float c1 = 1.70158;
  float c3 = c1 + 1;

  return 1 + c3 * pow(x - 1, 3) + c1 * pow(x - 1, 2);
}

float easeOutQuad(float x) {
  return 1 - (1 - x) * (1 - x);
}

float easeInOutQuad(float x) {
  return x < 0.5 ? 2 * x * x : 1 - pow(-2 * x + 2, 2) / 2;
}