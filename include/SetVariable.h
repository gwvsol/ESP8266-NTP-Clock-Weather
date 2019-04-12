#ifndef ESP8266WiFi_h
#define ESP8266WiFi_h
#endif
#ifndef Arduino_h
#define Arduino_h
#endif

// Определяем подключение к выводам микроконтроллера
#define LED 2                           // GPIO2 LED Board

// Установка языка индикации
uint8_t lang = 0; //0-RU, 1-BG, 2 -EN

// Настройка работы светодиодов
unsigned long lastTime      = 0;
unsigned long currentTime   = 0;

// Определяем константы для индикации режимов работы WiFi: ST или AP
const unsigned long WifiOn  = 100;       // Время включения светодиода wifi
const unsigned long WifiOff = 5000;      // Время выключения светодиода wifi
const unsigned long ApOn    = 1000;      // Время включения светодиода в режиме AP
const unsigned long ApOff   = 1000;      // Время выключения светодиода в режиме AP

// Определяем переменные для подключения к wifi
String ssid           = "w2234";            // Точка доступа для подключения
String passwd         = "Fedex##54";        // Пароль для подключения к точке доступа
String ssidAP         = "WiFiClock";        // Имя точки доступа в режиме AP
String passwdAP       = "rootroot";         // Пароль точки доступа в режиме AP
String jsonConfig     = "{}";               // Здесь храним сонфиг с файла и для его записи

// Настройки в режиме AP
IPAddress apIP(192, 168, 4, 1);         // IP адрес
IPAddress apGate(192, 168, 4, 1);       // IP адрес шлюза
IPAddress apNetMask(255, 255, 255, 0);  // Маска сети

// Настройка обноления времени с NTP сервера
String sNtpServerName = "pool.ntp.org";     // NTP сервер по умолчанию
uint8_t timezone      = 3;                  // часовой пояс GTM
const char* day_ru[] PROGMEM = {"Воскресенье", "Понедельник", "Вторник", "Среда", "Четверг", "Пятница", "Суббота"};
const char* day_bg[] PROGMEM = {"Неделя", "Понеделник", "Вторник", "Сряда", "Четвертък", "Петък", "Събота"};
const char* day_en[] PROGMEM = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const char** day_table[] PROGMEM = {day_ru, day_bg, day_en};
const char* month_ru[] PROGMEM = {"Января", "Февраля", "Марта", "Апреля", "Мая", "Июня", "Июля", "Августа", "Сентября", "Октября", "Ноября", "Декабря"};
const char* month_bg[] PROGMEM = {"Януари", "Февруари", "Март", "Април", "Май", "Юни", "Юли", "Август", "Септември", "Октомври", "Ноември", "Декември"};
const char* month_en[] PROGMEM = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
const char** month_table[] PROGMEM = {month_ru, month_bg, month_en};

// Настройка WiFi UDP для работы с NTP серверами для обновления времени
unsigned int localPort = 8888;  // Локальный порт для прослушивания UDP-пакетов