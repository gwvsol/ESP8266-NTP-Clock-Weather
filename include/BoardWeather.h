#include <ESP8266HTTPClient.h> 

HTTPClient client;

String ParseWeather(String weather){
    DynamicJsonDocument jsonDoc(1024);
    DeserializationError error =  deserializeJson(jsonDoc, weather);
    if (error) {
        Serial.print(F("deserializeJson() failed with code "));
        Serial.println(error.c_str());    
        Serial.println("Json parsing failed!");
        return "Weather error";
    } else {
        w_descript = jsonDoc["weather"][0]["description"].as<String>();       // Описание Погоды
        float t = jsonDoc["main"]["temp"].as<float>();
        char temperatureTemp[5];
        dtostrf(t, 5, 1, temperatureTemp);
        w_temp = String(temperatureTemp);                                     // Температура
        w_hum = jsonDoc["main"]["humidity"].as<String>();                     // Влажность
        double p = jsonDoc["main"]["pressure"].as<double>()/1.33322;
        w_pres = String((int)p);                                              // Давление
        double deg = jsonDoc["wind"]["deg"];
        if( deg >22.5 && deg <=67.5 )        w_wind = windir_table[lang][0];
        else if( deg >67.5 && deg <=112.5 )  w_wind = windir_table[lang][1];
        else if( deg >112.5 && deg <=157.5 ) w_wind = windir_table[lang][2];
        else if( deg >157.5 && deg <=202.5 ) w_wind = windir_table[lang][3];
        else if( deg >202.5 && deg <=247.5 ) w_wind = windir_table[lang][4];
        else if( deg >247.5 && deg <=292.5 ) w_wind = windir_table[lang][5];
        else if( deg >292.5 && deg <=337.5 ) w_wind = windir_table[lang][6];
        else w_wind = windir_table[lang][7];                                   // Направление Ветра
        w_speed = jsonDoc["wind"]["speed"].as<String>();                       // Скорость Ветра
        weather = "";
        // Формируем строку для вывода на экран
        w_out = overboard[lang]+w_descript+temper[lang]+w_temp+hum[lang];
        w_out += w_hum+pres[lang]+w_pres+wind[lang]+w_wind+w_speed+windsp[lang];
        Serial.println(w_out);
        return w_out; 
    }
}

// Получаем данные с сервера погоды
String Weather_init() { 
    String url;
    String l;
    if (lang == 0) l = "&lang=ru";
    else if (lang == 1) l = "&lang=bg";
    else if (lang == 2) l = "&lang=en";
    else l = "&lang=en";
    url = w_url+"?id="+city_id+"&APPID="+w_api+"&units=metric"+l;
    //Serial.println(url);
    Serial.println("Weather data update...");
    client.begin(url);
    int httpCode = client.GET();
    if (httpCode == HTTP_CODE_OK) {
        String httpString = client.getString(); 
        return ParseWeather(httpString);
    } else {
        return "";
    }
    client.end();
}

// Получаем данные о погоде
void GetWeather() {
    if (millis() - WeatherTime >= WeatherUpdate) {
        //Serial.println(WeatherTime);
        WeatherTime = millis();
        //Serial.println(WeatherTime);
        Weather_init();
    }
}