#include <ESP8266SSDP.h>
#ifndef ESP8266WiFi_h
#define ESP8266WiFi_h
#endif

void SSDP_init() {
    //Если версия  2.0.0 закаментируйте следующую строчку
    SSDP.setDeviceType(DeviceType);
    SSDP.setSchemaURL(SchemaURL);
    SSDP.setHTTPPort(WEBPORT);
    SSDP.setName(SSDP_Name);
    SSDP.setSerialNumber(SerialNumber);
    SSDP.setURL("/");
    SSDP.setModelName(ModelName);
    SSDP.setModelNumber(ModelNumber);
    SSDP.setModelURL(ModelURL);
    SSDP.setManufacturer(Manufacturer);
    SSDP.setManufacturerURL(ManufacturerURL);
    SSDP.begin();
}