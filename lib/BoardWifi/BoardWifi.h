/*
Подключение к WiFi или включение режима AP на ESP8266
*/
#ifndef BoardWifi_h
#define BoardWifi_h
#include <ESP8266WiFi.h>
#include <LedOut.h>

void WIFIinit(String ssid, String passwd, String ssidAP, String passwdAP);
void LedWiFi(int led, unsigned long wifion, unsigned long wifioff, unsigned long apon, unsigned long apoff);

#endif