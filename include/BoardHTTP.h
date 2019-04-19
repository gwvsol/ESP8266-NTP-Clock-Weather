#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#ifndef ESP8266WiFi_h
#define ESP8266WiFi_h
#endif
#ifndef ESP8266SSDP_h
#define ESP8266SSDP_h
#endif
#ifndef FS_h
#define FS_h
#endif

ESP8266WebServer HTTP(WEBPORT);         // Web интерфейс для устройства
ESP8266HTTPUpdateServer httpUpdater;    // Для обновления прошивки с web страницы 
File fsUploadFile;                      // Для работы с файловой системой

// Перезагрузка модуля по запросу вида http://IP/rst?rst=ok
void handle_Restart() {
    String restart = HTTP.arg("rst");
    if (restart == "ok") {
        HTTP.send(200, "text/plain", "Reset OK");
        ESP.restart();
    } else {
    HTTP.send(200, "text / plain", "No Reset"); // Oтправляем ответ No Reset
  }
}

// Принудительное сохранение файла конфигурации, необходимо в процессе отладки модуля
// По запросу по запросу вида http://IP/save?save=ok
void save_Config() {
    String save = HTTP.arg("save");
    if (save == "ok") {
        SaveConFig();
        HTTP.send(200, "text/plain", "OK");
    }
}

// Синхронизация времени устройства по запросу вида http://IP/time
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

// Установкаы времени устройства по запросу вида http://IP/tset?year=2019&month=12&day=25&hour=15&min=34&sec=45
void handle_TimeSet() {
    uint8_t new_y = HTTP.arg("year").toInt(); 
    uint8_t new_mth = HTTP.arg("month").toInt();
    uint8_t new_d = HTTP.arg("day").toInt();
    uint8_t new_h = HTTP.arg("hour").toInt(); 
    uint8_t new_m = HTTP.arg("min").toInt();
    uint8_t new_s = HTTP.arg("sec").toInt();
    useNTP = false;
    SaveConFig();
    setTime(new_h, new_m, new_s, new_d, new_mth, new_y); // Установка времени RTC микроконтроллера
    timeSynch();
    HTTP.send(200, "text/plain", "OK");                  // отправляем ответ о выполнении
}

// Включение синхронизации времени устройства по NTP серверу по запросу вида http://IP/usentp?use_sync=1 
// (выключение use_sync=0)
void handle_UseNTP() {
    int ntp = HTTP.arg("use_sync").toInt();
    if (ntp) useNTP = true;
    else useNTP = false;
    SaveConFig();
    timeSynch();
    HTTP.send(200, "text/plain", "OK");                 // отправляем ответ о выполнении
}

// Установка нового NTP сервера по запросу вида http://IP/setntp?setntpz=us.pool.ntp.org
void handle_ntp_server() {
    // Получаем значение нового NTP сервера и сохраняем
    NtpName = HTTP.arg("setntp").c_str();
    NtpName.toCharArray(ntpServerName, sizeof(ntpServerName));
    useNTP = true;
    SaveConFig();
    timeSynch();
    HTTP.send(200, "text/plain", "OK");
}

// Установка настроек будильника 1 по запросу вида http://IP/setalarm1?alarm1_h=12&alarm1_m=45
void handle_set_alarm1() {
    alarm1_hour = HTTP.arg("alarm1_h").toInt(); 
    alarm1_minute = HTTP.arg("alarm1_m").toInt();
    useAlarm1 = true;
    SaveConFig();
    timeSynch();
    HTTP.send(200, "text/plain", "OK");                  // отправляем ответ о выполнении
}

// Включение/выключение будильника 1 по запросу вида http://IP/usealarm1?use_alarm1=1 (выключение use_alarm1=0)
void handle_use_alarm1(){
    useAlarm1 = HTTP.arg("use_alarm1").toInt();
    SaveConFig();
    timeSynch();
    HTTP.send(200, "text/plain", "OK");                 // отправляем ответ о выполнении
}

// Установка настроек будильника 2 по запросу вида http://IP/setalarm2?alarm2_h=12&alarm2_m=45
void handle_set_alarm2() {
    alarm2_hour = HTTP.arg("alarm2_h").toInt(); 
    alarm2_minute = HTTP.arg("alarm2_m").toInt();
    useAlarm2 = true;
    SaveConFig();
    timeSynch();
    HTTP.send(200, "text/plain", "OK");                 // отправляем ответ о выполнении
}

// Включение/выключение будильника 2 по запросу вида http://IP/usealarm2?use_alarm2=1 (выключение use_alarm2=0)
void handle_use_alarm2(){
    useAlarm2 = HTTP.arg("use_alarm2").toInt();
    SaveConFig();
    timeSynch();
    HTTP.send(200, "text/plain", "OK");                 // отправляем ответ о выполнении
}

// Установка SSDP имени по запросу вида http://IP/ssdp?ssdp=proba
void handle_Set_Ssdp() {
    SSDP_Name = HTTP.arg("ssdp");       // Получаем значение ssdp из запроса сохраняем в глобальной переменной
    SaveConFig();                       // Функция сохранения данных во Flash
    HTTP.send(200, "text/plain", "OK"); // отправляем ответ о выполнении
}

// Установить имя и пароль AP по запросу вида http://IP/ssidap?ssidAP=home1&passwdAP=8765439
void handle_Set_Ssidap() {       
    ssidAP = HTTP.arg("ssidAP");         // Получаем значение ssidAP из запроса сохраняем в глобальной переменной
    passwdAP = HTTP.arg("passwordAP");   // Получаем значение passwordAP из запроса сохраняем в глобальной переменной
    SaveConFig();                        // Функция сохранения данных во Flash
    HTTP.send(200, "text/plain", "OK");  // отправляем ответ о выполнении
}

// Установить имя и пароль роутера по запросу вида http://IP/ssid?ssid=home2&passwd=12345678
void handle_Set_Ssid() {
    ssid = HTTP.arg("ssid");             // Получаем значение ssid из запроса сохраняем в глобальной переменной
    passwd = HTTP.arg("password");       // Получаем значение password из запроса сохраняем в глобальной переменной
    SaveConFig();                        // Функция сохранения данных во Flash
    HTTP.send(200, "text/plain", "OK");  // отправляем ответ о выполнении
}

// Установка языка системы по запросу вида http://IP/lang?lang=0 //0-RU, 1-BG, 2 -EN
void handle_Set_Lang() {
    lang = HTTP.arg("lang").toInt();
    SaveConFig();
    //strWeather = GetWeather();         // После изменения языка, необходимо обновить данные о погоде на соотв.языке
    HTTP.send(200, "text/plain", "OK");  // отправляем ответ о выполнении
}

// Получаем значение яркости экрана по запросу вида http://IP/setbright?bright=3
void handle_Brightness() {
    brightness = HTTP.arg("bright").toInt();
    SaveConFig();
    //matrix.setIntensity(brightness); // Передача настроек на экран
    HTTP.send(200, "text/plain", "OK");  // отправляем ответ о выполнении
}

// Получаем значение скорости вывода информации на экран
// по запросу http://IP/setspeed?speed_d=100
void handle_Speed() {
    delaySym = HTTP.arg("speed_d").toInt();
    SaveConFig();
    HTTP.send(200, "text/plain", "OK");  // отправляем ответ о выполнении
}

// Получаем произвольную строку для вывода на экран
// по запросу http://IP/text?text=test-test
void handle_Text() {
    strText = HTTP.arg("text");
    SaveConFig();
    HTTP.send(200, "text/plain", "OK");  // отправляем ответ о выполнении
}

// Получаем значение настроек о получении погоды с сервера
// по запросу http://IP/weather?city_code=city_code&w_api=w_api
void handle_Weather() {
    city_id = HTTP.arg("city_id");
    w_api = HTTP.arg("w_api");
    SaveConFig();
    HTTP.send(200, "text/plain", "OK");  // отправляем ответ о выполнении
}

// Включение обновления погоды по запросу вида
// http://IP/useweath?use_weath=1 (выключение use_weath=0)
void use_Weather() {
    useWeather = HTTP.arg("use_weath").toInt();
    SaveConFig();
    HTTP.send(200, "text/plain", "OK");  // отправляем ответ о выполнении
}

// Синхронизация погоды по запросу http://IP/updateweath
void update_Weather() {
    Weather_init();
    HTTP.send(200, "text/plain", "OK"); // Отправляем ответ о выполнении
}

// Методы для работы с SPIFFS
String getContentType(String filename) {
    //if (HTTP.hasArg("download")) return "application/octet-stream";
    //else if (filename.endsWith(".htm")) return "text/html";
    if (filename.endsWith(".htm")) return "text/html";
    else if (filename.endsWith(".html")) return "text/html";
    else if (filename.endsWith(".json")) return "application/json";
    else if (filename.endsWith(".css")) return "text/css";
    else if (filename.endsWith(".js")) return "application/javascript";
    else if (filename.endsWith(".png")) return "image/png";
    else if (filename.endsWith(".gif")) return "image/gif";
    else if (filename.endsWith(".jpg")) return "image/jpeg";
    else if (filename.endsWith(".ico")) return "image/x-icon";
    else if (filename.endsWith(".xml")) return "text/xml";
    else if (filename.endsWith(".pdf")) return "application/x-pdf";
    else if (filename.endsWith(".zip")) return "application/x-zip";
    else if (filename.endsWith(".gz")) return "application/x-gzip";
    return "text/plain";
}

bool handleFileRead(String path) {
    if (path.endsWith("/")) {
        if (lang == 0) path += "index-ru.htm";
        else if (lang == 1) path += "index-bg.htm";
        else if (lang == 2) path += "index-en.htm";
        else path += "index-en.htm";
    }
    if (path.endsWith("/setting")) {
        if (lang == 0) path += "setting-ru.htm";
        else if (lang == 1) path += "setting-bg.htm";
        else if (lang == 2) path += "setting-en.htm";
        else path += "setting-en.htm";
    }
    String contentType = getContentType(path);
    String pathWithGz = path + ".gz";
    if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
        if (SPIFFS.exists(pathWithGz))
        path += ".gz";
        File file = SPIFFS.open(path, "r");
        size_t sent = HTTP.streamFile(file, contentType);
        file.close();
        return true;
    }
    return false;
}

//void handleFileUpload() {
//    if (HTTP.uri() != "/edit") return;
//    HTTPUpload& upload = HTTP.upload();
//    if (upload.status == UPLOAD_FILE_START) {
//        String filename = upload.filename;
//        if (!filename.startsWith("/")) filename = "/" + filename;
//        fsUploadFile = SPIFFS.open(filename, "w");
//        filename = String();
//    } else if (upload.status == UPLOAD_FILE_WRITE) {
//        //DBG_OUTPUT_PORT.print("handleFileUpload Data: "); DBG_OUTPUT_PORT.println(upload.currentSize);
//        if (fsUploadFile)
//        fsUploadFile.write(upload.buf, upload.currentSize);
//    } else if (upload.status == UPLOAD_FILE_END) {
//        if (fsUploadFile)
//        fsUploadFile.close();
//    }
//}

//void handleFileDelete() {
//    if (HTTP.args() == 0) return HTTP.send(500, "text/plain", "BAD ARGS");
//    String path = HTTP.arg(0);
//    if (path == "/")
//        return HTTP.send(500, "text/plain", "BAD PATH");
//    if (!SPIFFS.exists(path))
//        return HTTP.send(404, "text/plain", "FileNotFound");
//    SPIFFS.remove(path);
//    HTTP.send(200, "text/plain", "");
//    path = String();
//}

//void handleFileCreate() {
//    if (HTTP.args() == 0)
//        return HTTP.send(500, "text/plain", "BAD ARGS");
//    String path = HTTP.arg(0);
//    if (path == "/")
//        return HTTP.send(500, "text/plain", "BAD PATH");
//    if (SPIFFS.exists(path))
//        return HTTP.send(500, "text/plain", "FILE EXISTS");
//    File file = SPIFFS.open(path, "w");
//    if (file)
//        file.close();
//    else
//        return HTTP.send(500, "text/plain", "CREATE FAILED");
//    HTTP.send(200, "text/plain", "");
//    path = String();
//}

//void handleFileList() {
//    if (!HTTP.hasArg("dir")) {
//        HTTP.send(500, "text/plain", "BAD ARGS");
//        return;
//    }
//    String path = HTTP.arg("dir");
//    Dir dir = SPIFFS.openDir(path);
//    path = String();
//    String output = "[";
//    while (dir.next()) {
//        File entry = dir.openFile("r");
//        if (output != "[") output += ',';
//        bool isDir = false;
//        output += "{\"type\":\"";
//        output += (isDir) ? "dir" : "file";
//        output += "\",\"name\":\"";
//        output += String(entry.name()).substring(1);
//        output += "\"}";
//        entry.close();
//    }
//    output += "]";
//    HTTP.send(200, "text/json", output);
//}

// Вывод данных config.json 
void handle_ConfigJSON() {
    time_t tn = now();                  // Получение времени на микроконтроллере
    DynamicJsonDocument jsonDoc(1024);
    // Заполняем поля json
    jsonDoc["SSDP"]         = SSDP_Name;
    jsonDoc["ssidAP"]       = ssidAP;
    jsonDoc["passwordAP"]   = passwdAP;
    jsonDoc["ssid"]         = ssid;
    jsonDoc["password"]     = passwd;
    jsonDoc["timezone"]     = timezone;
    if (WiFi.status() != WL_CONNECTED)  {
        jsonDoc["ip"]       = WiFi.softAPIP().toString(); 
    } else {
        jsonDoc["ip"]       = WiFi.localIP().toString();
    }
    if (useNTP) jsonDoc["use_sync"] = "checked";
    else        jsonDoc["use_sync"] = "";
    jsonDoc["date_y"]       = String(year(tn));
    jsonDoc["date_m"]       = String(month(tn));
    jsonDoc["date_d"]       = String(day(tn));
    jsonDoc["time_h"]       = String(hour(tn));
    jsonDoc["time_m"]       = String(minute(tn));
    jsonDoc["time_s"]       = String(second(tn));
    jsonDoc["time"]         = GetTime();
    jsonDoc["date"]         = GetDate();
    jsonDoc["ntpserver"]    = NtpName; 
    jsonDoc["lang"]         = lang;
    if (useAlarm1) jsonDoc["use_alarm1"] = "checked";
    else           jsonDoc["use_alarm1"] = "";
    jsonDoc["alarm1_h"]     = alarm1_hour;
    jsonDoc["alarm1_m"]     = alarm1_minute;
    if (useAlarm2) jsonDoc["use_alarm2"] = "checked";
    else           jsonDoc["use_alarm2"] = "";
    jsonDoc["alarm2_h"]     = alarm2_hour;
    jsonDoc["alarm2_m"]     = alarm2_minute;
    jsonDoc["bright"]       = brightness;
    jsonDoc["speed_d"]      = delaySym;
    jsonDoc["text"]         = strText;
    jsonDoc["w_api"]        = w_api;
    jsonDoc["city_id"]      = city_id;
    jsonDoc["w_descript"]   = w_descript;
    jsonDoc["w_temp"]       = w_temp;
    jsonDoc["w_hum"]        = w_hum;
    jsonDoc["w_pres"]       = w_pres;
    jsonDoc["w_wind"]       = w_wind;
    jsonDoc["w_speed"]      = w_speed;
    jsonDoc["city_name"]    = city_name;
    if (useWeather) jsonDoc["use_weather"] = "checked";
    else            jsonDoc["use_weather"] = "";
    // Помещаем созданный json в переменную root
    String htmlOut;
    serializeJson(jsonDoc, htmlOut);
    HTTP.send(200, "text/json", htmlOut);
}

// Метод инициализации Web сервиса
void HTTP_init(void) {
    // Формирование configs.json страницы для передачи данных в web интерфейс
    HTTP.on("/configs.json", handle_ConfigJSON);
    // Перезагрузка модуля по запросу вида http://IP/restart?rst=ok
    HTTP.on("/rst", handle_Restart);
    // Установить имя и пароль роутера по запросу вида http://IP/ssid?ssid=home2&password=12345678
    HTTP.on("/ssid", handle_Set_Ssid);
    // Установить имя и пароль AP по запросу вида http://IP/ssidap?ssidAP=home1&passwordAP=8765439
    HTTP.on("/ssidap", handle_Set_Ssidap);
    // Установить имя SSDP устройства по запросу вида http://IP/ssdp?ssdp=proba
    HTTP.on("/ssdp", handle_Set_Ssdp);
    // Установка языка системы по запросу вида http://IP/lang?lang=0 //0-RU, 1-BG, 2 -EN
    HTTP.on("/lang", handle_Set_Lang);
    // Синхронизация времени устройства по запросу вида http://IP/time
    HTTP.on("/time", handle_Time);
    // Установка времени устройства по запросу вида 
    // http://IP/tset?year=2019&month=12&day=25&hour=15&min=34&sec=45
    HTTP.on("/tset", handle_TimeSet);
    // Включение синхронизации времени устройства по NTP серверу по запросу вида 
    // http://IP/usentp?use_sync=1 (выключение use_sync=0)
    HTTP.on("/usentp", handle_UseNTP);
    // Установка временной зоны по запросу вида http://IP/timez?timez=3
    HTTP.on("/timez", handle_time_zone);
    // Установка нового NTP сервера по запросу вида http://IP/setntp?setntpz=us.pool.ntp.org
    HTTP.on("/setntp", handle_ntp_server);
    // Принудительное сохранение config файла по запросу вида http://IP/save?save=ok
    HTTP.on("/save", save_Config);
    // Установка настроек будильника 1 по запросу вида http://IP/setalarm1?alarm1_h=12&alarm1_m=45
    HTTP.on("/setalarm1", handle_set_alarm1);
    // Включение/выключение будильника 1 по запросу вида http://IP/usealarm1?use_alarm1=1 (выключение use_alarm1=0)
    HTTP.on("/usealarm1", handle_use_alarm1);
    // Установка настроек будильника 2 по запросу вида http://IP/setalarm2?alarm2_h=12&alarm2_m=45
    HTTP.on("/setalarm2", handle_set_alarm2);
    // Включение/выключение будильника 2 по запросу вида http://IP/usealarm2?use_alarm2=1 (выключение use_alarm2=0)
    HTTP.on("/usealarm2", handle_use_alarm2);
    // Установка яркости экрана часов
    HTTP.on("/setbright", handle_Brightness);
    // Установка скорости информации на экране
    HTTP.on("/setspeed", handle_Speed);
    // Установка произвольного текста на экране 
    HTTP.on("/text", handle_Text);
    // Установка настроек погоды по запросу вида
    // http://IP/weather?city_id=732770&w_api=f266126b1c5cf63858b5f713a25908da
    HTTP.on("/weather", handle_Weather);
    // Включение обновления погоды по запросу вида
    // http://IP/useweath?use_weath=1 (выключение use_weath=0)
    HTTP.on("/useweath", use_Weather);
    // Синхронизация погоды по запросу http://IP/updateweath
    HTTP.on("/updateweath", update_Weather);
    // SSDP дескриптор по запросу http://IP/description.xml
    HTTP.on("/description.xml", HTTP_GET, []() {SSDP.schema(HTTP.client()); });
    //==============================================================================
    // HTTP страницы для работы с SPIFFS
    //HTTP.on("/list", HTTP_GET, handleFileList);     // list directory
    //HTTP.on("/edit", HTTP_GET, []() {
    //    if (!handleFileRead("/edit.htm")) HTTP.send(404, "text/plain", "FileNotFound");
    //});                                                 // Загрузка редактора editor
    //HTTP.on("/edit", HTTP_PUT, handleFileCreate);       // Создание файла
    //HTTP.on("/edit", HTTP_DELETE, handleFileDelete);    // Удаление файла
    // first callback is called after the request has ended with all parsed arguments
    // second callback handles file uploads at that location
    //HTTP.on("/edit", HTTP_POST, []() {
    //    HTTP.send(200, "text/plain", "");
    //}, handleFileUpload);
    // called when the url is not defined here
    // use it to load content from SPIFFS
    //==============================================================================
    HTTP.onNotFound([]() {
        if (!handleFileRead(HTTP.uri()))
        HTTP.send(404, "text/plain", "FileNotFound"); });   // Файл не найден
    // Обновление так же возможно с помощью утилиты curl 
    // Пример использования curl -F "image=@firmware.bin" IP/update
    httpUpdater.setup(&HTTP);                               // Для обнавления прошивки с web страницы 
    HTTP.begin();
}