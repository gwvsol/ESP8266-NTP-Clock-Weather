#ifndef ESP8266WiFi_h
#define ESP8266WiFi_h
#endif

// Метод для включения в режиме AP
bool StartAPMode() {
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apGate, apNetMask);
  WiFi.softAP(ssidAP.c_str(), passwdAP.c_str());
  return true;
}

// Индикация режима работы AP или ST
void LedWiFi(int led) {
  if (WiFi.status() == WL_CONNECTED) {
    ledWifi(led, WifiOn, WifiOff); // Режим подключения к точке доступа
  } else {
    ledWifi(led, ApOn, ApOff);     // Режим AP
  }
}

void WIFIinit() {
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
    StartAPMode();
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
  }
}

