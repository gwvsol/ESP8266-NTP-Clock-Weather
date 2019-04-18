#ifndef Arduino_h
#define Arduino_h
#endif

void ledToggle(int led) {
  digitalWrite(led, !digitalRead(led));  // Change the state of the LED
}

void ledWifi(int led, unsigned long on, unsigned long off){
    unsigned long dtime = 0;
    if (digitalRead(led) == HIGH) {
        dtime = off;
    } else {
        dtime = on;
    }
    if (millis() - LedTime >= dtime) {
        LedTime = millis();
        ledToggle(led);
    }
}