#include "wifi_setup.h"
#include "display.h"
#include <ESP8266WiFi.h>

void connectWiFi(Adafruit_SSD1306 &display, WiFiManager &wifiManager,
                 WiFiManagerParameter &customDataEndpoint, char *dataEndpoint,
                 DeviceState &state)
{
    wifiManager.addParameter(&customDataEndpoint);
    wifiManager.setConfigPortalTimeout(60);

    auto displayAPCallback = [&display](WiFiManager *mgr)
    {
        showAPScreen(display);
    };
    wifiManager.setAPCallback(displayAPCallback);

    if (state.displayOk)
    {
        showConnectingScreen(display);
    }

    if (!wifiManager.autoConnect("AutoConnectAP"))
    {
        if (state.displayOk)
        {
            showErrorScreen(display);
            delay(2000);
        }
        return;
    }

    strcpy(dataEndpoint, customDataEndpoint.getValue());

    if (state.displayOk)
    {
        showSuccessScreen(display, dataEndpoint);
        delay(3000);
    }
}

void showAPScreen(Adafruit_SSD1306 &display)
{
    display.clearDisplay();
    drawHeader(display, "WiFi");
    display.println("To configure wi-fi");
    display.println("connect to hotspot:");
    display.println("> AutoConnectAP");
    drawFooter(display, "Timeout: 60s");
}

void showConnectingScreen(Adafruit_SSD1306 &display)
{
    display.clearDisplay();
    drawHeader(display, "WiFi");
    display.println("Connecting to:");
    display.print("> ");
    display.println(WiFi.SSID());
    drawFooter(display, "Please wait...");
}

void showErrorScreen(Adafruit_SSD1306 &display)
{
    display.clearDisplay();
    drawHeader(display, "Error");
    display.println("Connection failed");
    display.println("Timeout reached");
    drawFooter(display, "No WiFi available");
}

void showSuccessScreen(Adafruit_SSD1306 &display, const char *dataEndpoint)
{
    display.clearDisplay();
    drawHeader(display, "WiFi");
    display.println("Connected!");
    display.print("> ");
    display.println(WiFi.localIP());
    drawFooter(display, "Ready");
}
