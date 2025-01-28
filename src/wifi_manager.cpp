#include <WiFiManager.h>

#include <ESP8266WiFi.h>

#include "sensors.h"
#include "display.h"
#include "wifi_manager.h"

WiFiManager wifiManager;

void showAPScreen()
{
    display.clearDisplay();
    drawHeader("WiFi");

    display.setCursor(0, 20);
    display.println("To configure wi-fi");
    display.println("connect to hotspot:");
    display.println("> AutoConnectAP");

    display.display();
    display.setCursor(0, 0);
}

void showConnectingScreen()
{
    display.clearDisplay();
    drawHeader("WiFi");

    display.setCursor(0, 20);
    display.println("Connecting to:");
    display.print("> ");
    display.println(WiFi.SSID());
    drawFooter("Please wait...");

    display.display();
    display.setCursor(0, 0);
}

void showSuccessScreen()
{
    display.clearDisplay();
    drawHeader("WiFi");
    display.setCursor(0, 20);

    display.println("Connected!");
    display.print("> ");
    display.println(WiFi.localIP());
    drawFooter("Ready");

    display.display();
    display.setCursor(0, 0);
}

void connectWiFi()
{
    wifiManager.setConfigPortalBlocking(false);

    auto displayAPCallback = [](WiFiManager *mgr)
    {
        showAPScreen();
        delay(3000);
    };
    wifiManager.setAPCallback(displayAPCallback);

    showConnectingScreen();

    if (wifiManager.autoConnect("AutoConnectAP"))
    {
        showSuccessScreen();
        delay(3000);
    }
}
