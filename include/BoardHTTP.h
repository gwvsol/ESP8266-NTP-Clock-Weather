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

// Установка нового NTP сервера по запросу вида http://IP/setntp?setntpz=us.pool.ntp.org
void handle_ntp_server() {
    // Получаем значение нового NTP сервера и сохраняем         
    NtpName = HTTP.arg("setntp").c_str();
    NtpName.toCharArray(ntpServerName, sizeof(ntpServerName));
    Serial.println(NtpName);
    SaveConFig();
    timeSynch();
    HTTP.send(200, "text/plain", "OK");
}

// Методы для работы с SPIFFS
String getContentType(String filename) {
    if (HTTP.hasArg("download")) return "application/octet-stream";
    else if (filename.endsWith(".htm")) return "text/html";
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
    if (path.endsWith("/")) path += "index.htm";
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

void handleFileUpload() {
    if (HTTP.uri() != "/edit") return;
    HTTPUpload& upload = HTTP.upload();
    if (upload.status == UPLOAD_FILE_START) {
        String filename = upload.filename;
        if (!filename.startsWith("/")) filename = "/" + filename;
        fsUploadFile = SPIFFS.open(filename, "w");
        filename = String();
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        //DBG_OUTPUT_PORT.print("handleFileUpload Data: "); DBG_OUTPUT_PORT.println(upload.currentSize);
        if (fsUploadFile)
        fsUploadFile.write(upload.buf, upload.currentSize);
    } else if (upload.status == UPLOAD_FILE_END) {
        if (fsUploadFile)
        fsUploadFile.close();
    }
}

void handleFileDelete() {
    if (HTTP.args() == 0) return HTTP.send(500, "text/plain", "BAD ARGS");
    String path = HTTP.arg(0);
    if (path == "/")
        return HTTP.send(500, "text/plain", "BAD PATH");
    if (!SPIFFS.exists(path))
        return HTTP.send(404, "text/plain", "FileNotFound");
    SPIFFS.remove(path);
    HTTP.send(200, "text/plain", "");
    path = String();
}

void handleFileCreate() {
    if (HTTP.args() == 0)
        return HTTP.send(500, "text/plain", "BAD ARGS");
    String path = HTTP.arg(0);
    if (path == "/")
        return HTTP.send(500, "text/plain", "BAD PATH");
    if (SPIFFS.exists(path))
        return HTTP.send(500, "text/plain", "FILE EXISTS");
    File file = SPIFFS.open(path, "w");
    if (file)
        file.close();
    else
        return HTTP.send(500, "text/plain", "CREATE FAILED");
    HTTP.send(200, "text/plain", "");
    path = String();
}

void handleFileList() {
    if (!HTTP.hasArg("dir")) {
        HTTP.send(500, "text/plain", "BAD ARGS");
        return;
    }
    String path = HTTP.arg("dir");
    Dir dir = SPIFFS.openDir(path);
    path = String();
    String output = "[";
    while (dir.next()) {
        File entry = dir.openFile("r");
        if (output != "[") output += ',';
        bool isDir = false;
        output += "{\"type\":\"";
        output += (isDir) ? "dir" : "file";
        output += "\",\"name\":\"";
        output += String(entry.name()).substring(1);
        output += "\"}";
        entry.close();
    }
    output += "]";
    HTTP.send(200, "text/json", output);
}

// Вывод данных config.json 
void handle_ConfigJSON() {
    time_t tn = now();                  // Получение времени на контроллере
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
    jsonDoc["time"]         = GetTime();
    jsonDoc["date"]         = GetDate();
    jsonDoc["ntpserver"]    = NtpName; 
    // Помещаем созданный json в переменную root
    String htmlOut;
    serializeJson(jsonDoc, htmlOut);
    HTTP.send(200, "text/json", htmlOut);
}

void HTTP_init(void) {
    // Формирование configs.json страницы для передачи данных в web интерфейс
    HTTP.on("/configs.json", handle_ConfigJSON);
    // Перезагрузка модуля по запросу вида http://IP/restart?rst=ok
    HTTP.on("/rst", handle_Restart);
    // Синхронизация времени устройства по запросу вида http://IP/time
    HTTP.on("/time", handle_Time);
    // Установка временной зоны по запросу вида http://IP/timez?timez=3
    HTTP.on("/timez", handle_time_zone);
    // Установка нового NTP сервера по запросу вида http://IP/setntp?setntpz=us.pool.ntp.org
    HTTP.on("/setntp", handle_ntp_server);
    // HTTP страницы для работы с SPIFFS
    HTTP.on("/list", HTTP_GET, handleFileList);     // list directory
    HTTP.on("/edit", HTTP_GET, []() {
        if (!handleFileRead("/edit.htm")) HTTP.send(404, "text/plain", "FileNotFound");
    });                                                 // Загрузка редактора editor
    HTTP.on("/edit", HTTP_PUT, handleFileCreate);       // Создание файла
    HTTP.on("/edit", HTTP_DELETE, handleFileDelete);    // Удаление файла
    // first callback is called after the request has ended with all parsed arguments
    // second callback handles file uploads at that location
    HTTP.on("/edit", HTTP_POST, []() {
        HTTP.send(200, "text/plain", "");
    }, handleFileUpload);
    // called when the url is not defined here
    // use it to load content from SPIFFS
    HTTP.onNotFound([]() {
        if (!handleFileRead(HTTP.uri()))
        HTTP.send(404, "text/plain", "FileNotFound");
    });                                                 // Файл не найден
    // Принудительное сохранение config файла по запросу вида http://IP/save?save=ok
    HTTP.on("/save", save_Config);
    // SSDP дескриптор
    HTTP.on("/description.xml", HTTP_GET, []() {SSDP.schema(HTTP.client()); });
    HTTP.begin();
}