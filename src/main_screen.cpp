#include "display.h"
#include <ESP8266WiFi.h>
#include "sensors.h"

const unsigned int SENSOR_READING_SWITCH_INTERVAL = 1500;
const unsigned int FOOTER_MESSAGE_SWITCH_INTERVAL = 3000;

static unsigned long lastFooterSwitchMillis = 0;
static unsigned long lastReadingSwitchMillis = 0;

static unsigned int readingIndex = 0;
static bool showWiFiText = true;

void showMainScreen()
{
    display.clearDisplay();

    if (millis() - lastReadingSwitchMillis > SENSOR_READING_SWITCH_INTERVAL)
    {
        readingIndex = (readingIndex + 1) % 3;
        lastReadingSwitchMillis = millis();
    }

    const char *readingTypes[3] = {"T \xF7"
                                   "C",
                                   "RH %", "CO2"};
    drawHeader(readingTypes[readingIndex], TextAlign::LEFT);

    SensorReading averageReadings = calculateAverages();

    const char *status;
    if (readingIndex == 0) // Temperature
    {
        if (averageReadings.temperature < 18)
            status = "LOW";
        else if (averageReadings.temperature > 24)
            status = "HIGH";
        else
            status = "OK";
    }
    else if (readingIndex == 1) // Humidity
    {
        if (averageReadings.humidity < 40)
            status = "LOW";
        else if (averageReadings.humidity > 60)
            status = "HIGH";
        else
            status = "OK";
    }
    else // CO2
    {
        if (averageReadings.co2 > 1000)
            status = "HIGH";
        else
            status = "OK";
    }
    drawHeader(status, TextAlign::RIGHT);

    char readingValue[10];
    if (readingIndex == 0)
    {
        snprintf(readingValue, sizeof(readingValue), "%.1f", averageReadings.temperature);
    }
    else if (readingIndex == 1)
    {
        snprintf(readingValue, sizeof(readingValue), "%.1f", averageReadings.humidity);
    }
    else
    {
        snprintf(readingValue, sizeof(readingValue), "%d", averageReadings.co2);
    }

    display.setTextSize(4);
    int xPos = calculateTextX(readingValue, 4, TextAlign::CENTER);
    display.setCursor(xPos, 21);
    display.print(readingValue);

    bool wifiConnected = (WiFi.status() == WL_CONNECTED);
    if (wifiConnected)
    {
        if (millis() - lastFooterSwitchMillis > SENSOR_READING_SWITCH_INTERVAL)
        {
            showWiFiText = !showWiFiText;
            lastFooterSwitchMillis = millis();
        }
        if (showWiFiText)
        {
            drawFooter("WiFi connected");
        }
        else
        {
            String ip = WiFi.localIP().toString();
            drawFooter(ip.c_str());
        }
    }
    else
    {
        drawFooter("WiFi disconnected");
    }

    display.display();
}