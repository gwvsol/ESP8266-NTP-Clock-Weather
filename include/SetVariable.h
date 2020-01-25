#ifndef ESP8266WiFi_h
#define ESP8266WiFi_h
#endif
#ifndef Arduino_h
#define Arduino_h
#endif

// Определяем подключение к выводам микроконтроллера
#define LED 2                           // GPIO2 LED Board

// Определяем скорость UART порта
#define UARTSPEED 115200

// Определяем порт для WEB Сервера
#define WEBPORT 80

// Определяем порт для WEB UDP
#define WEBUDP 8888

// Установка языка индикации
uint8_t lang = 0; //0-RU, 1-BG, 2 -EN // Параметр обновляется с web интерфейса

// Настройка переменной со значением времени
//unsigned long currentTime      = millis();
unsigned long NtpTime         = 0;
const unsigned long NtpUpdate = 3600000;  // Интервал обновления web сервиса

// Определяем константы для индикации режимов работы WiFi: ST или AP
unsigned long LedTime     = 0;
const unsigned long WifiOn  = 100;       // Время включения светодиода wifi
const unsigned long WifiOff = 5000;      // Время выключения светодиода wifi
const unsigned long ApOn    = 1000;      // Время включения светодиода в режиме AP
const unsigned long ApOff   = 1000;      // Время выключения светодиода в режиме AP

// Определяем переменные для подключения к wifi // Все Параметры обновляется с web интерфейса
String ssid           = "FRESH_ASUS";            // Точка доступа для подключения
String passwd         = "FRESH250391";        // Пароль для подключения к точке доступа
String ssidAP         = "WiFiClock";        // Имя точки доступа в режиме AP
String passwdAP       = "rootroot";         // Пароль точки доступа в режиме AP

// Настройки для сервиса SSDP
String SSDP_Name      = "CLOCK.Lightwell";  // Имя SSDP // Параметр обновляется с web интерфейса
String DeviceType     = "upnp:rootdevice";
String SchemaURL      = "description.xml";
String SerialNumber   = "001788102205";
String ModelName      = "SSDP-LW";
String ModelNumber    = "000001000001";
String ModelURL       = "https://led-lightwell.eu/";
String Manufacturer   = "LIGHTWELL";
String ManufacturerURL = "https://led-lightwell.eu";

// Настройки в режиме AP
IPAddress apIP(192, 168, 4, 1);               // IP адрес
IPAddress apGate(192, 168, 4, 1);             // IP адрес шлюза
IPAddress apNetMask(255, 255, 255, 0);        // Маска сети

// Настройка обноления времени с NTP сервера
String NtpName          = "pool.ntp.org";     // NTP сервер по умолчанию // Параметр обновляется с web интерфейса
uint8_t timezone        = 3;                  // часовой пояс GTM // Параметр обновляется с web интерфейса
bool useNTP             = true;               // Разрешаем обновление времени с NTP сервера
const char* day_ru[] PROGMEM = {"Воскресенье", "Понедельник", "Вторник", "Среда", "Четверг", "Пятница", "Суббота"};
const char* day_bg[] PROGMEM = {"Неделя", "Понеделник", "Вторник", "Сряда", "Четвертък", "Петък", "Събота"};
const char* day_en[] PROGMEM = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const char** day_table[] PROGMEM = {day_ru, day_bg, day_en};
const char* month_ru[] PROGMEM = {"Января", "Февраля", "Марта", "Апреля", "Мая", "Июня", "Июля", "Августа", "Сентября", "Октября", "Ноября", "Декабря"};
const char* month_bg[] PROGMEM = {"Януари", "Февруари", "Март", "Април", "Май", "Юни", "Юли", "Август", "Септември", "Октомври", "Ноември", "Декември"};
const char* month_en[] PROGMEM = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
const char** month_table[] PROGMEM = {month_ru, month_bg, month_en};

// Настройка WiFi UDP для работы с NTP серверами для обновления времени
unsigned int localPort = WEBUDP;              // Локальный порт для прослушивания UDP-пакетов

// Настройка работы 2 будильников
bool useAlarm1           = false;             // Параметр обновляется с web интерфейса
uint8_t alarm1_hour      = 0;                 // Параметр обновляется с web интерфейса
uint8_t alarm1_minute    = 0;                 // Параметр обновляется с web интерфейса
bool useAlarm2           = false;             // Параметр обновляется с web интерфейса
uint8_t alarm2_hour      = 0;                 // Параметр обновляется с web интерфейса
uint8_t alarm2_minute    = 0;                 // Параметр обновляется с web интерфейса

// Настройки работы индикатора часов
uint8_t brightness       = 6;                // Яркость свечения экрана // Параметр обновляется с web интерфейса
uint16_t delaySym        = 150;               // Скорость бегущей строки, задержка сдвига одного символа


// Произвольный текст на экране индикатора
String strText           = "Online watch Lightwell";  // Переменная для хранения произвольного текста для вывода на экран

// Настройки для работы с сервисом погоды
const char* overboard[] PROGMEM = {"Погода на улице: ", "Времето навън: ", "The weather outside: "};
const char* temper[] PROGMEM = {". Темп:", ". Темп:", ". Temperature:"};
const char* hum[] PROGMEM = {"`С. Влажн: ", "`С. Влажн: ", "`С. Humidity: "};
const char* pres[] PROGMEM = {"%. Давл: ", "%. Налягане: ", "%. Pressure: "};
const char* wind[] PROGMEM = {" мм. Ветер ", " мм. Вятър ", " mm. Wind "};
const char* windsp[] PROGMEM = {" м/с.", " м/с.", " m/s"};
const char* windir_ru[] PROGMEM = {"северо-вост ", "вост ", "юго-вост ", "южный ", "юго-запад ", "запад ", "северо-запад ", "север "};
const char* windir_bg[] PROGMEM = {"север-изт ", "източ ", "юго-източ ", "южен ", "южен-запад ", "запад ", "север.-запад ", "север "};
const char* windir_en[] PROGMEM = {"North-East ", "East ", "South-East ", "South ", "South-West ", "West ", "North-West ", "North "};
const char** windir_table[] PROGMEM = {windir_ru, windir_bg, windir_en};

unsigned long WeatherTime           = 0;
bool useWeather                     = true;              // Параметр обновляется с web интерфейса
const unsigned long WeatherUpdate   = 90000;              // Интервал обновления погоды
String strWeather        = "Expect weather data updates"; //
String w_api             = "f266126b1c5cf63858b5f713a25908da";  // API OpenWeatherMap
// Коды городов http://bulk.openweathermap.org/sample/city.list.json.gz
String city_id           = "732770";
String w_url             = "http://api.openweathermap.org/data/2.5/weather";
String w_out             = "";
String w_descript        = "";
String w_temp            = "";
String w_hum             = "";
String w_pres            = "";
String w_wind            = "";
String w_speed           = "";
String city_name         = "";