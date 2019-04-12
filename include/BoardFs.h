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
    //json["SSDPName"] = SSDP_Name;
    json["ssidAP"] = ssidAP;
    json["passwdAP"] = passwdAP;
    json["ssid"] = ssid;
    json["passwd"] = passwd;
    //json["timezone"] = timezone;
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
    // загружаем файл конфигурации в глобальную переменную
    jsonConfig = configFile.readString();
    // Резервируем память для json обекта буфер может рости по мере необходимти ESP8266 
    DynamicJsonBuffer jsonBuffer;
    //  вызовите парсер JSON через экземпляр jsonBuffer
    //  строку возьмем из глобальной переменной String jsonConfig
    JsonObject& root = jsonBuffer.parseObject(jsonConfig);
    // Теперь можно получить значения из root  
    ssidAP = root["ssidAP"].as<String>(); // Так получаем строку
    passwdAP = root["passwdAP"].as<String>();
    //timezone = root["timezone"];               // Так получаем число
    //SSDP_Name = root["SSDPName"].as<String>();
    ssid = root["ssid"].as<String>();
    passwd = root["passwd"].as<String>();
    configFile.close(); // Закрываем файл
    return true;
}