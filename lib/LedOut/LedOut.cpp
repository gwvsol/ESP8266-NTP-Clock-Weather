#include "LedOut.h"

unsigned long previousMillis = 0;

void ledToggle(int led) {
  digitalWrite(led, !digitalRead(led));  // Change the state of the LED
}

void ledwifi(int led, unsigned long on, unsigned long off){
    unsigned long dtime = 0;
    unsigned long currentMillis = millis();
    if (digitalRead(led) == HIGH) {
        dtime = off;
    } else {
        dtime = on;
    }
    if (currentMillis - previousMillis >= dtime) {
        previousMillis = currentMillis;
        ledToggle(led);
    }
}