#include "main.h"

#include "display.h"
#include "sensors.h"
#include "wifi_manager.h"
#include "web_server.h"
#include "data_endpoint.h"

const unsigned int RX_PIN = 3;
const unsigned int TX_PIN = 1;

const unsigned int SDA_PIN = 0;
const unsigned int SCL_PIN = 2;

const unsigned long POST_INTERVAL = 5000;
const unsigned int SENSOR_UPDATE_INTERVAL = 1000;

unsigned long lastPostMillis = 0;

SoftwareSerial mySerial(RX_PIN, TX_PIN);

void setup()
{
  Wire.begin(SDA_PIN, SCL_PIN);
  initDisplay();
  initSHT4();
  initMHZ19();
  connectWiFi();
  server.begin();
}

void loop()
{
  handleClient();

  unsigned long currentMillis = millis();
  if (currentMillis - state.lastUpdate >= SENSOR_UPDATE_INTERVAL)
  {
    state.lastUpdate = currentMillis;
    updateSensors();
    updateDisplay();
  }

  if (currentMillis - lastPostMillis >= POST_INTERVAL)
  {
    lastPostMillis = currentMillis;
    sendDataToServer();
  }
}
