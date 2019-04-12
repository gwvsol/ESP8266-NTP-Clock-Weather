#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SetVariable.h>
#include <BoardFs.h>
#include <LedOut.h>
#include <BoardWifi.h>
#include <BoardTimeNtp.h>

void setup() {
    Serial.begin(115200);              // Настраиваем UART порт для вывода отладочной информации
    Serial.println("");
    Serial.println("===================");
    Serial.println("WiFi LED: Pin2 (Board LED)");
    pinMode(LED, OUTPUT);              // Настраиваем Pin2 на выход
    digitalWrite(LED, HIGH);           // Выключаем светодиод
    Serial.println("Set FS configuration");
    FS_init();                         // Настраиваем работу файловой системы
    //SaveConFig();
    Serial.println("Read the data from the configuration file");
    LoadConFig();                      // Загружаем файл настроек 
    Serial.println("Set WiFi:");
    WIFIinit();                        // Подключаемся к WiFi
    Time_init();
}

void loop() {
    LedWiFi(LED);                      // Индикация прдключения к WiFi
}
