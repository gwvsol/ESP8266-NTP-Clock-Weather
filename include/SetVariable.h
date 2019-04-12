#ifndef ESP8266WiFi_h
#define ESP8266WiFi_h
#endif
#ifndef Arduino_h
#define Arduino_h
#endif

// Определяем подключение к выводам микроконтроллера
#define LED 2                           // GPIO2 LED Board

// Настройка работы светодиодов
unsigned long lastTime      = 0;
unsigned long currentTime   = 0;

// Определяем константы для индикации режимов работы WiFi: ST или AP
const unsigned long WifiOn  = 100;       // Время включения светодиода wifi
const unsigned long WifiOff = 5000;      // Время выключения светодиода wifi
const unsigned long ApOn    = 1000;      // Время включения светодиода в режиме AP
const unsigned long ApOff   = 1000;      // Время выключения светодиода в режиме AP

// Определяем переменные wifi
String ssid       = "w2234";       // Точка доступа для подключения
String passwd     = "Fedex##54";   // Пароль для подключения к точке доступа
String ssidAP     = "WiFiClock";   // Имя точки доступа в режиме AP
String passwdAP   = "rootroot";    // Пароль точки доступа в режиме AP
String jsonConfig = "{}"; // Здесь храним сонфиг с файла и для его записи

// Настройки в режиме AP
IPAddress apIP(192, 168, 4, 1);         // IP адрес
IPAddress apGate(192, 168, 4, 1);       // IP адрес шлюза
IPAddress apNetMask(255, 255, 255, 0);  // Маска сети