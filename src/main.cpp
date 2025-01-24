#include <Arduino.h>
#include <Adafruit_SHT4x.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
#include <DNSServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <MHZ19.h>
#include <SoftwareSerial.h>
#include <WiFiManager.h>
#include <Wire.h>

#include "display.h"
#include "sensors.h"
#include "wifi_setup.h"
#include "web_server.h"
#include "data_endpoint.h"

// Global objects
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SHT4x sht4;
MHZ19 myMHZ19;
SoftwareSerial mySerial(RX_PIN, TX_PIN);
WiFiServer server(80);
WiFiManager wifiManager;

DeviceState state;
AveragingBuffer buffer;
char dataEndpoint[80];
WiFiManagerParameter customDataEndpoint("dataEndpoint", "Data endpoint",
                                        dataEndpoint, 80);

unsigned long lastPostMillis = 0;
const unsigned long POST_INTERVAL = 5000;

void setup()
{
  Wire.begin(SDA_PIN, SCL_PIN);
  initDisplay(display, state);
  initSHT4(sht4, state);
  initMHZ19(mySerial, myMHZ19, state);
  connectWiFi(display, wifiManager,
              customDataEndpoint, dataEndpoint, state);
  server.begin();
}

void loop()
{
  handleClient(server, wifiManager, display, state);

  unsigned long currentMillis = millis();
  if (currentMillis - state.lastUpdate >= SENSOR_UPDATE_INTERVAL)
  {
    state.lastUpdate = currentMillis;
    updateSensors(sht4, myMHZ19, buffer, state);
    updateDisplay(display, state);
  }

  if (currentMillis - lastPostMillis >= POST_INTERVAL)
  {
    lastPostMillis = currentMillis;
    sendDataToServer(dataEndpoint, buffer, display, state);
  }
}
