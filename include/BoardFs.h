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
    DynamicJsonBuffer jsonBuffer;
    //  вызовите парсер JSON через экземпляр jsonBuffer
    JsonObject& json = jsonBuffer.parseObject(jsonConfig);
    // Заполняем поля json 
    json["SSDP"]    = SSDP_Name;
    json["AP"]      = ssidAP;
    json["pwdAP"]   = passwdAP;
    json["ssid"]    = ssid;
    json["pwd"]     = passwd;
    json["tz"]      = timezone;
    json["ntp"]     = sNtpServerName;
    json["lang"]    = lang;
    // Помещаем созданный json в глобальную переменную json.printTo(jsonConfig);
    json.printTo(jsonConfig);
    // Открываем файл для записи
    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
        Serial.println("Failed to open config file for writing");
        return false;
    }
    // Записываем config json в файл и закрываем его
    json.printTo(configFile);
    configFile.close();
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
    jsonConfig = configFile.readString();
    // Serial.println(jsonConfig);  // Для отладки, можно увидесть что записано в файле конфигурации
    // Резервируем память для json обекта буфер может рости по мере необходимти ESP8266 
    DynamicJsonBuffer jsonBuffer;
    // Парсер JSON через jsonBuffer
    JsonObject& root = jsonBuffer.parseObject(jsonConfig);
    // Получаем значения из переменной root  
    ssidAP      = root["AP"].as<String>();
    passwdAP    = root["pwdAP"].as<String>();
    timezone    = root["tz"];
    lang        = root["lang"];
    sNtpServerName = root["ntp"].as<String>();
    ssid        = root["ST"].as<String>();
    passwd      = root["pwd"].as<String>();
    SSDP_Name   = root["SSDP"].as<String>();
    configFile.close();                         // Закрываем файл
    return true;
}