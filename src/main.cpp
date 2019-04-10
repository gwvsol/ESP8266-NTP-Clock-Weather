#include <Arduino.h>
#include <BoardWifi.h>

// Определяем подключение к выводам микроконтроллера
#define LED 2                          // GPIO2 LED Board

// Определяем константы для индикации режимов работы WiFi: ST или AP
const unsigned long wifion  = 100;     // Время включения светодиода wifi
const unsigned long wifioff = 5000;    // Время выключения светодиода wifi
const unsigned long apon  = 1000;      // Время включения светодиода в режиме AP
const unsigned long apoff = 1000;      // Время выключения светодиода в режиме AP

// Определяем переменные wifi
String ssid      = "w2234";       // Точка доступа для подключения
String passwd    = "Fedex##54";   // Пароль для подключения к точке доступа
String ssidAP    = "WiFiClock";   // Имя точки доступа в режиме AP
String passwdAP  = "rootroot";    // Пароль точки доступа в режиме AP


void setup() {
  Serial.begin(115200);              // Настраиваем UART порт для вывода отладочной информации
  Serial.println("");
  Serial.println("===================");
  Serial.println("WiFi LED: Pin2 (Board LED)");
  pinMode(LED, OUTPUT);              // Настраиваем Pin2 на выход
  digitalWrite(LED, HIGH);           // Выключаем светодиод
  Serial.println("Set WiFi:");
  WIFIinit(ssid, passwd, ssidAP, passwdAP);
}

void loop() {
  LedWiFi(LED, wifion, wifioff, apon, apoff);
}