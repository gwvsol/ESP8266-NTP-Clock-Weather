/*
Использование SPIFFS на ESP8266
*/
#ifndef BoardFs_h
#define BoardFs_h
#include <FS.h>

void WIFIinit(String ssid, String passwd, String ssidAP, String passwdAP);

#endif