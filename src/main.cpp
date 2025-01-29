#include "main.h"

#include "display.h"
#include "sensors.h"
#include "wifi_manager.h"
#include "web_server.h"
#include "data_endpoint.h"
#include "main_screen.h"

const unsigned int RX_PIN = 3;
const unsigned int TX_PIN = 1;
const unsigned int SDA_PIN = 0;
const unsigned int SCL_PIN = 2;

const unsigned int POST_INTERVAL = 5000;
const unsigned int SENSOR_UPDATE_INTERVAL = 5000;
const unsigned int MAIN_SCREEN_REFRESH_INTERVAL = 100;

static unsigned long lastPostMillis = 0;
static unsigned long lastSensorsUpdateMillis = 0;
static unsigned long lastMainScreenRefreshMillis = 0;

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
  wifiManager.process();
  handleClient();

  unsigned long currentMillis = millis();

  if (currentMillis - lastSensorsUpdateMillis >= SENSOR_UPDATE_INTERVAL)
  {
    lastSensorsUpdateMillis = currentMillis;
    updateSensors();
  }

  if (currentMillis - lastMainScreenRefreshMillis >= MAIN_SCREEN_REFRESH_INTERVAL)
  {
    lastMainScreenRefreshMillis = currentMillis;
    showMainScreen();
  }

  if (currentMillis - lastPostMillis >= POST_INTERVAL)
  {
    lastPostMillis = currentMillis;
    sendDataToServer();
  }
}
