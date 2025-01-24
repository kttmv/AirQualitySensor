#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

#include <WiFiManager.h>
#include <Adafruit_SSD1306.h>
#include "sensors.h"

void connectWiFi(Adafruit_SSD1306 &display, WiFiManager &wifiManager,
                 WiFiManagerParameter &customDataEndpoint, char *dataEndpoint,
                 DeviceState &state);

#endif
