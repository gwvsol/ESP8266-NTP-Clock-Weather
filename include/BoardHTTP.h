#include <ESP8266WebServer.h>
#ifndef ESP8266WiFi_h
#define ESP8266WiFi_h
#endif
#ifndef ESP8266SSDP_h
#define ESP8266SSDP_h
#endif
#ifndef FS_h
#define FS_h
#endif

ESP8266WebServer HTTP(WEBPORT);      // Web интерфейс для устройства
File fsUploadFile;                   // Для работы с файловой системой

// Ответ при обращении к главной странице
void handleRoot() {
    HTTP.send(200, "text/plain", "hello from esp8266!");
}

// Перезагрузка модуля по запросу вида http://IP/restart?device=ok
void handle_Restart() {
    String restart = HTTP.arg("rst");
    if (restart == "ok") {
        HTTP.send(200, "text/plain", "DEVICE RESTART OK");
        ESP.restart();
    }
}

void handle_Time() {
    timeSynch();
    HTTP.send(200, "text/plain", "OK"); // Отправляем ответ о выполнении
}

// Установка параметров времянной зоны по запросу вида http://IP/timez?timez=3
void handle_time_zone() {
    // Получаем значение timezone из запроса конвертируем в int сохраняем в глобальной переменной           
    timezone = HTTP.arg("timez").toInt();
    SaveConFig();
    timeSynch();
    HTTP.send(200, "text/plain", "OK");
}

// Установка нового NTP сервера по запросу вида http://IP/setntp?setntpz=us.pool.ntp.org
void handle_ntp_server() {
    // Получаем значение нового NTP сервера и сохраняем         
    sNtpServerName = HTTP.arg("setntp");
    sNtpServerName.toCharArray(ntpServerName, sizeof(ntpServerName));
    SaveConFig();
    timeSynch();
    HTTP.send(200, "text/plain", "OK");
}

void HTTP_init(void) {
    // Главная страница http://IP/
    HTTP.on("/", handleRoot);
    // Перезагрузка модуля по запросу вида http://IP/restart?rst=ok
    HTTP.on("/restart", handle_Restart);
    // Синхронизация времени устройства по запросу вида http://IP/time
    HTTP.on("/time", handle_Time);
    // Установка временной зоны по запросу вида http://IP/timez?timez=3
    HTTP.on("/timez", handle_time_zone);
    // Установка нового NTP сервера по запросу вида http://IP/setntp?setntpz=us.pool.ntp.org
    HTTP.on("/setntp", handle_ntp_server);
    // SSDP дескриптор
    HTTP.on("/description.xml", HTTP_GET, []() {SSDP.schema(HTTP.client()); });
    HTTP.begin();
}