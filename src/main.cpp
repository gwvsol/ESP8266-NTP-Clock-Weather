#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SetVariable.h>
#include <BoardFs.h>
#include <LedOut.h>
#include <BoardWifi.h>
#include <BoardTimeNtp.h>
#include <BoardSSDP.h>
#include <BoardHTTP.h>
#include <BoardWeather.h>

void setup() {
    Serial.begin(UARTSPEED);              // Настраиваем UART порт для вывода отладочной информации
    Serial.println("");
    Serial.println("===================");
    Serial.println("WiFi LED: Pin2 (Board LED)");
    pinMode(LED, OUTPUT);              // Настраиваем Pin2 на выход
    digitalWrite(LED, HIGH);           // Выключаем светодиод
    Serial.println("Set FS configuration");
    FS_init();                         // Настраиваем работу файловой системы
    //SaveConFig();                      // Для отладки, можно принудительно перезаписать файл конфигурации
    Serial.println("Read the data from the configuration file");
    LoadConFig();                      // Загружаем файл настроек 
    Serial.println("Set WiFi");
    WIFI_init();                        // Подключаемся к WiFi
    Serial.println("Set Time");
    Time_init();
    Serial.println("Set SSDP Service");
    SSDP_init();
    Serial.println("Set and Start Web Service");
    HTTP_init();
    Weather_init();
}

void loop() {
    LedWiFi(LED);                      // Индикация прдключения к WiFi
    HTTP.handleClient();
    delay(1);
    GetWeather();
    GetNTP();
}