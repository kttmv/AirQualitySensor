#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <Adafruit_SSD1306.h>
#include "sensors.h"

void handleClient(WiFiServer &server, WiFiManager &wifiManager,
                  Adafruit_SSD1306 &display, DeviceState &state);

#endif
