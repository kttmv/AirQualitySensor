#include "wifi_setup.h"
#include <ESP8266WiFi.h>

#include "wifi_setup.h"
#include <ESP8266WiFi.h>

void connectWiFi(Adafruit_SSD1306 &display, WiFiManager &wifiManager,
                 WiFiManagerParameter &customDataEndpoint, char *dataEndpoint,
                 DeviceState &state)
{
    wifiManager.addParameter(&customDataEndpoint);

    wifiManager.setConfigPortalTimeout(60);

    auto displayAPCallback = [&display](WiFiManager *mgr)
    {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Connect to access point:");
        display.println("AutoConnectAP");
        display.println("Timeout: 60 seconds");
        display.display();
    };

    wifiManager.setAPCallback(displayAPCallback);

    if (state.displayOk)
    {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("Connecting to ");
        display.println(WiFi.SSID());
        display.display();
    }

    if (!wifiManager.autoConnect("AutoConnectAP"))
    {
        if (state.displayOk)
        {
            display.clearDisplay();
            display.setCursor(0, 0);
            display.println("Timeout reached");
            display.println("Continuing without WiFi");
            display.display();
            delay(2000);
        }
        return;
    }

    strcpy(dataEndpoint, customDataEndpoint.getValue());

    if (state.displayOk)
    {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("WiFi connected!");
        display.println("IP address:");
        display.println(WiFi.localIP());
        display.println("Data endpoint:");
        display.println(dataEndpoint);
        display.display();
        delay(3000);
    }
}
