#include "wifi_setup.h"
#include <ESP8266WiFi.h>

void connectWiFi(Adafruit_SSD1306 &display, WiFiManager &wifiManager,
                 WiFiManagerParameter &customDataEndpoint, char *dataEndpoint,
                 DeviceState &state)
{
    if (state.displayOk)
    {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Connect to access point:");
        display.println("AutoConnectAP");
        display.display();
    }

    wifiManager.addParameter(&customDataEndpoint);
    wifiManager.autoConnect("AutoConnectAP");

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
