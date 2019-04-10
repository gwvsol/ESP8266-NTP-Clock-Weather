#include "BoardWifi.h"

// Настройки в режиме AP
IPAddress apIP(192, 168, 4, 1);         // IP адрес
IPAddress apGate(192, 168, 4, 1);       // IP адрес шлюза
IPAddress apNetMask(255, 255, 255, 0);  // Маска сети

// Метод для включения в режиме AP
bool StartAPMode(String ssidAP, String passwdAP) {
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apGate, apNetMask);
  WiFi.softAP(ssidAP.c_str(), passwdAP.c_str());
  return true;
}

// Индикация режима работы AP или ST
void LedWiFi(int led, unsigned long wifion, unsigned long wifioff, unsigned long apon, unsigned long apoff) {
  if (WiFi.status() == WL_CONNECTED) {
    ledwifi(led, wifion, wifioff); // Режим подключения к точке доступа
  } else {
    ledwifi(led, apon, apoff);     // Режим AP
  }
}

void WIFIinit(String ssid, String passwd, String ssidAP, String passwdAP) {
  // Попытка подключения к точке доступа
  WiFi.mode(WIFI_STA);
  byte tries = 11;
  WiFi.begin(ssid.c_str(), passwd.c_str());
  while (--tries && WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) { 
    // Если успешное подключение к точке доступа
    Serial.println("");
    Serial.println("WiFi up ST");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP()); 
  } else {
    // Если подключение к точке доступа не удалось включаем в режиме AP
    Serial.println("");
    Serial.println("WiFi up AP");
    StartAPMode(ssidAP, passwdAP);
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
  }
}