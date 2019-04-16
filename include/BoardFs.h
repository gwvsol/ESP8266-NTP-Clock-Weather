#include <FS.h>
#include <ArduinoJson.h>

// Инициализация файловой системы SPIFFS
void FS_init(void) {
    SPIFFS.begin(); {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
        String fileName = dir.fileName();
        size_t fileSize = dir.fileSize();
        }
    FSInfo fs_info;
    SPIFFS.info(fs_info);
    Serial.print("Total: ");
    Serial.print(fs_info.totalBytes/1024);
    Serial.println(" Kb");
    Serial.print("Used: ");
    Serial.print(fs_info.usedBytes/1024);
    Serial.println(" Kb");
    Serial.print("Free: ");
    Serial.print(fs_info.totalBytes/1024 - fs_info.usedBytes/1024);
    Serial.println(" Kb");
    }
}

// Запись данных в файл config.json
bool SaveConFig() {
    // Резервируем памяь для json обекта буфер может рости по мере необходимти ESP8266
    DynamicJsonDocument json(1024);
    // Заполняем поля json 
    json["SSDP"]    = SSDP_Name;
    json["AP"]      = ssidAP;
    json["pwdAP"]   = passwdAP;
    json["ST"]      = ssid;
    json["pwd"]     = passwd;
    json["tz"]      = timezone;
    json["ntp"]     = NtpName;
    json["lang"]    = lang;
    // Открываем файл для записи
    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
        Serial.println("Failed to open config file for writing");
        return false;
    }
    // Записываем config json в файл и закрываем его
    serializeJson(json, configFile);
    configFile.close();
    Serial.println("Save config file OK!");
    return true;
}

// Загрузка данных сохраненных в файле config.json
bool LoadConFig() { // Открываем файл для чтения
    File configFile = SPIFFS.open("/config.json", "r");
    if (!configFile) {  // если файл не найден  
        Serial.println("Failed to open config file");
        //  Создаем файл запиав в него даные по умолчанию
        SaveConFig();
        return false;
        }
    size_t size = configFile.size();
    if (size > 1024) {  // Проверяем размер файла, будем использовать файл размером меньше 1024 байта
        Serial.println("Config file size is too large");
        return false;
        }
    // Загружаем файл конфигурации в глобальную переменную
    String jsonConfig     = "{}";                // Здесь храним конфиг с файла и для его записи
    jsonConfig = configFile.readString();
    Serial.println("Load Config File...");
    Serial.println(jsonConfig);  // Для отладки, можно увидесть что записано в файле конфигурации
    // Резервируем память для json обекта буфер может рости по мере необходимти ESP8266
    DynamicJsonDocument doc(1024);
    // Парсер JSON через jsonBuffer
    deserializeJson(doc, jsonConfig);
    // JsonObject root = jsonBuffer.parseObject(jsonConfig);
    // Получаем значения из переменной root  
    ssidAP      = doc["AP"].as<String>();
    passwdAP    = doc["pwdAP"].as<String>();
    timezone    = doc["tz"];
    lang        = doc["lang"];
    NtpName = doc["ntp"].as<String>();
    ssid        = doc["ST"].as<String>();
    passwd      = doc["pwd"].as<String>();
    SSDP_Name   = doc["SSDP"].as<String>();
    configFile.close();                         // Закрываем файл
    return true;
}