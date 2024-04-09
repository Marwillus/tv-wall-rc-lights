#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

const int FAST_DELAY = 2000;
const int SLOW_DELAY = 3000;

int LED_1 = 9;
int LED_2 = 10;
int POTI_PIN = 2;

bool powerOn = false;
int mode = 0;
bool nextMode = false;

int potiValue = 0;
int prevOffsetPotiValue = 0;
int minPotiValue = 5;
int maxPotiValue = 950;

int brightness = 0;
int brightness1 = 0;
int brightness2 = 0;
int maxBrightness = 100;

unsigned long now;         // Store current millis().
unsigned long prevOffset;  // store last measured millis().
unsigned long deltaT = 0;
int prevFade = 50;
int fadeInterval = 20;
// float t1 = 0.0;
// float t2 = 0.0;

int fadeStep = 0;

void setup() {
  // Serial.begin(9600);
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
      // fading up --> led1 for 2s, pause for 1s, led2 for 1s
      fadeUp();
      break;
    case 2:
      //fading down --> all at same time
      fadeDown();
      break;

    default:
      break;
  }
}

void startOffsetTimer() {
  prevOffset = now;
}

void fadeUp() {
  if (fadeStep == 0) {
    if (now - prevFade >= fadeInterval) {
      prevFade = now;
      static float t1 = 0.0;
      const float duration = 1500.0;

      // Serial.print("Fadeup 1: ");
      // Serial.print(t1);
      // Serial.print(", ");
      // Serial.print("brightness1: ");
      // Serial.println(brightness1);

      if (t1 <= 1.0) {
        brightness1 = (int)(maxBrightness * easeOutBack(t1));
        analogWrite(LED_1, brightness1);
        t1 += fadeInterval / duration;
      } else {
        t1 = 0.0;
        fadeStep++;
      }
    }
  }
  // offset delay
  if (now - prevOffset >= 1000) {
    if (fadeStep == 1) {
      if (now - prevFade >= fadeInterval) {
        prevFade = now;
        static float t2 = 0.0;
        const float duration = 4000.0;

        // Serial.print("Fadeup 2: ");
        // Serial.print(t2);
        // Serial.print(", ");
        // Serial.print("brightness2: ");
        // Serial.println(brightness2);

        if (t2 <= 1.0) {
          brightness2 = (int)(maxBrightness * easeOutQuad(t2));
          analogWrite(LED_2, brightness2);
          t2 += fadeInterval / duration;
        } else {
          t2 = 0.0;
          fadeStep++;
        }
      }
    }
  }
  if (now - prevOffset >= 7000 && fadeStep == 2) {
    blink();
    fadeStep++;
  }
  if (fadeStep == 3) {
    mode = 0;
    fadeStep = 0;
  }
}

void fadeDown() {
  if (now - prevFade >= fadeInterval) {
    prevFade = now;
    static float t3 = 1.0;
    const float duration = 4000.0;

    if (t3 >= 0.0) {
      brightness1 = (int)(maxBrightness * easeInOutQuad(t3));
      brightness2 = (int)(maxBrightness * easeOutQuad(t3));
      analogWrite(LED_1, brightness1);
      analogWrite(LED_2, brightness2);
      t3 -= fadeInterval / duration;
      // Serial.print("Fade down: ");
      // Serial.println(t3);
    }

    if (t3 < 0.0) {
      t3 = 1.0;
      mode = 0;
      fadeStep = 0;
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
      startOffsetTimer();
      powerOn = true;
      mode = 1;
      // Serial.println("on");
    }
    if (mySwitch.getReceivedValue() == 8685773 || mySwitch.getReceivedValue() == 8685775) {
      if (powerOn) {
        startOffsetTimer();
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
  if (prevOffsetPotiValue > potiValue + 4 || prevOffsetPotiValue < potiValue - 4) {
    // Serial.println(potiValue);

    // adjust max min value of poti
    if (potiValue < minPotiValue) {
      minPotiValue = potiValue;
    }
    if (potiValue > maxPotiValue) {
      maxPotiValue = potiValue;
    }

    maxBrightness = map(potiValue, minPotiValue, maxPotiValue, 18, 255);

    if (powerOn) {
      analogWrite(LED_1, maxBrightness);
      analogWrite(LED_2, maxBrightness);
    }
    prevOffsetPotiValue = potiValue;
  }
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