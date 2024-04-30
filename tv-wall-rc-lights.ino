#include <Arduino.h>
#include <RCSwitch.h>

// Constants
#define DEFAULT_FADE_INTERVAL 20

// Ease functions
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

// LED class
class LED {
public:
    LED(int pin, float amplitude = 1.0) 
        : pin(pin), amplitude(amplitude), brightness(0),
          fadeComplete(true), t(0.0) {
        pinMode(pin, OUTPUT);
    }

    void setBrightness(int value) {
        brightness = constrain(value, 0, 255);
        analogWrite(pin, brightness * amplitude);
    }

    int getBrightness() const {
        return brightness;
    }

    void fadeUp(int duration, int interval = DEFAULT_FADE_INTERVAL) {
        if (fadeComplete) {
            fadeComplete = false;
            t = 0.0;
        }

        if (millis() - lastFadeTime >= interval && t <= 1.0) {
            lastFadeTime = millis();
            int newBrightness = int(255 * easeOutBack(t));
            setBrightness(newBrightness);
            t += float(interval) / duration;

            if (t > 1.0) {
                fadeComplete = true;
            }
        }
    }

    void fadeDown(int duration, int interval = DEFAULT_FADE_INTERVAL) {
        if (fadeComplete) {
            fadeComplete = false;
            t = 1.0;
        }

        if (millis() - lastFadeTime >= interval && t >= 0.0) {
            lastFadeTime = millis();
            int newBrightness = int(255 * easeInOutQuad(t));
            setBrightness(newBrightness);
            t -= float(interval) / duration;

            if (t < 0.0) {
                fadeComplete = true;
            }
        }
    }

    bool isFadeComplete() const {
        return fadeComplete;
    }

private:
    int pin;
    float amplitude;
    int brightness;
    bool fadeComplete;
    float t;
    unsigned long lastFadeTime = 0;
};

// Main program
RCSwitch mySwitch;
LED led1(9);
LED led2(10, 0.9);

bool powerOn = false;
int mode = 0;

unsigned long now;
unsigned long prevDelay;
unsigned long prevFadeLed1;
unsigned long prevFadeLed2;

void setup() {
    mySwitch = RCSwitch();
    mySwitch.enableReceive(1);  // Receiver on interrupt 0 => that is pin #2
}

void loop() {
    now = millis();
    if (mySwitch.available()) {
        int receivedValue = mySwitch.getReceivedValue();
        if (receivedValue == 8685765 || receivedValue == 8685767) {
            powerOn = true;
            mode = 1;
            mySwitch.resetAvailable();
        } else if (receivedValue == 8685773 || 8685775) {
            powerOn = false;
            mode = 2;
            mySwitch.resetAvailable();
        }
    }

    if (powerOn) {
        switch (mode) {
            case 1:
                led1.fadeUp(3000);
                led2.fadeUp(6000);
                if (led1.isFadeComplete() && led2.isFadeComplete()) {
                    mode = 0;
                }
                break;

            case 2:
                led1.fadeDown(1500);
                led2.fadeDown(1500);
                if (led1.isFadeComplete() && led2.isFadeComplete()) {
                    mode = 0;
                }
                break;

            default:
                break;
        }
    }
}