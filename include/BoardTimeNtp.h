#include <TimeLib.h> //https://github.com/PaulStoffregen/Time Michael Margolis
#include <WiFiUdp.h>

WiFiUDP Udp;
const int NTP_PACKET_SIZE = 48;         //  NTP-время – в первых 48 байтах сообщения
byte packetBuffer[NTP_PACKET_SIZE];     //  буфер для хранения входящих и исходящих пакетов
char ntpServerName[50];

// отправляем NTP-запрос серверу времени по указанному адресу: 
void sendNTPpacket(IPAddress &address) {
    // задаем все байты в буфере на «0»: 
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    // инициализируем значения для создания NTP-запроса
    packetBuffer[0] = 0b11100011;   // LI (от «leap indicator», т.е. «индикатор перехода»), версия, режим работы 
    packetBuffer[1] = 0;            // слой (или тип часов) 
    packetBuffer[2] = 6;            // интервал запросов 
    packetBuffer[3] = 0xEC;         // точность 
    // 8 байтов с нулями, обозначающие базовую задержку и базовую дисперсию: 
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;
    // После заполнения всех указанных полей вы сможете отправлять пакет с запросом о временной метке:      
    Udp.beginPacket(address, 123); // NTP-запросы к порту 123
    Udp.write(packetBuffer, NTP_PACKET_SIZE);
    Udp.endPacket();
}

time_t getNtpTime() {
    IPAddress ntpServerIP;                          // IP-адрес NTP-сервера
    while (Udp.parsePacket() > 0) ;                 // отбраковываем все пакеты, полученные ранее 
    Serial.println("Transmit NTP Request");         //  "Передача NTP-запроса" 
    WiFi.hostByName(ntpServerName, ntpServerIP);    // подключаемся к случайному серверу из списка:
    Serial.print(ntpServerName);
    Serial.print(": ");
    Serial.println(ntpServerIP);
    sendNTPpacket(ntpServerIP);
    uint32_t beginWait = millis();
    while (millis() - beginWait < 1500) {
        int size = Udp.parsePacket();
        if (size >= NTP_PACKET_SIZE) {
            Serial.println("Receive NTP Response");   // "Получение NTP-ответа"
            Udp.read(packetBuffer, NTP_PACKET_SIZE);  // считываем пакет в буфер 
            unsigned long secsSince1900;
            // конвертируем 4 байта (начиная с позиции 40) в длинное целое число: 
            secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
            secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
            secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
            secsSince1900 |= (unsigned long)packetBuffer[43];
            return secsSince1900 - 2208988800UL + timezone * SECS_PER_HOUR;
        }
    }
    Serial.println("No NTP Response :-(");             //  "Нет NTP-ответа :(" 
    return 0;                                          // если время получить не удалось, возвращаем «0» 
}

// Получение текущего времени
String GetTime() {
    time_t tn = now();
    String Time = String(hour(tn))+":"+ String(minute(tn)) +":"\
        + String(second(tn));    // Строка с результатом времени
    return Time; // Возврашаем полученное время
}

// Получение даты
String GetDate() {
    time_t tn = now();
    String Date = String(day(tn))+" "+month_table[lang][month(tn)-1] +" "+String(year(tn)) + \
        ", " + day_table[lang][weekday(tn)-1];
    return Date; // Возврашаем полученную дату
}

void timeSynch() {
    setSyncInterval(300);
    if (WiFi.status() == WL_CONNECTED) { //только если есть подключение
        setSyncProvider(getNtpTime);
    }
    Serial.println("ITime Ready!");
    Serial.println(GetTime());
    Serial.println(GetDate());
}

// Начальная инициализация времени при включении
void Time_init() {
    sNtpServerName.toCharArray(ntpServerName, sizeof(ntpServerName));
    //Serial.println(sNtpServerName);
    Serial.println("Starting UDP");           // "Начинаем UDP"
    Udp.begin(localPort);
    Serial.print("Local port: ");             //  "Локальный порт: "
    Serial.println(Udp.localPort());
    Serial.println("Waiting for sync Time");  // "Ожидаем синхронизации времени"
    timeSynch();
}