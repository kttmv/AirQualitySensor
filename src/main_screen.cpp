#include "main_screen.h"
#include "display.h"
#include <ESP8266WiFi.h>
#include "sensors.h"

const unsigned int SENSOR_READING_SWITCH_INTERVAL = 1500;
const unsigned int FOOTER_MESSAGE_SWITCH_INTERVAL = 4000;
const unsigned int CUSTOM_MESSAGE_DISPLAY_TIME = 2000;

const char *const WIFI_ENABLED_MESSAGES[] = {
    "WiFi connected",
    "IP: {LOCAL_IP}"};
const size_t WIFI_ENABLED_MESSAGES_COUNT = sizeof(WIFI_ENABLED_MESSAGES) / sizeof(WIFI_ENABLED_MESSAGES[0]);

const char *const WIFI_DISABLED_MESSAGES[] = {
    "WiFi disconnected",
    "Connect to hotspot:",
    "AutoConnectAP"};
const size_t WIFI_DISABLED_MESSAGES_COUNT = sizeof(WIFI_DISABLED_MESSAGES) / sizeof(WIFI_DISABLED_MESSAGES[0]);

static unsigned long lastReadingSwitchMillis = 0;

static unsigned int readingIndex = 0;

static FooterState footerState;

void showCustomFooterMessage(const char *message)
{
    footerState.customMessages.push(std::string(message));
}

void updateFooter()
{
    bool wifiConnected = (WiFi.status() == WL_CONNECTED);
    unsigned long currentMillis = millis();

    if (!footerState.customMessages.empty())
    {
        if (footerState.customMessageStartTime == 0)
        {
            footerState.customMessageStartTime = currentMillis;
        }

        if (currentMillis - footerState.customMessageStartTime >= CUSTOM_MESSAGE_DISPLAY_TIME)
        {
            footerState.customMessages.pop();
            footerState.customMessageStartTime = 0;
        }

        if (!footerState.customMessages.empty())
        {
            drawFooter(footerState.customMessages.front().c_str());
            return;
        }
    }

    if (currentMillis - footerState.lastSwitchMillis >= FOOTER_MESSAGE_SWITCH_INTERVAL)
    {
        footerState.messageIndex++;
        footerState.lastSwitchMillis = currentMillis;

        if (wifiConnected)
        {
            footerState.messageIndex %= WIFI_ENABLED_MESSAGES_COUNT;
        }
        else
        {
            footerState.messageIndex %= WIFI_DISABLED_MESSAGES_COUNT;
        }
    }

    if (wifiConnected)
    {
        const char *message = WIFI_ENABLED_MESSAGES[footerState.messageIndex];
        if (strstr(message, "{LOCAL_IP}") != nullptr)
        {
            String ip = WiFi.localIP().toString();
            String tempMessage = String(message);
            tempMessage.replace("{LOCAL_IP}", ip);
            drawFooter(tempMessage.c_str());
        }
        else
        {
            drawFooter(message);
        }
    }
    else
    {
        const char *message = WIFI_DISABLED_MESSAGES[footerState.messageIndex];
        drawFooter(message);
    }
}

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

    updateFooter();

    display.display();
}
