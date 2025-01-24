#include "wifi_setup.h"
#include <ESP8266WiFi.h>

void connectWiFi(Adafruit_SSD1306 &display, WiFiManager &wifiManager,
                 WiFiManagerParameter &customDataEndpoint, char *dataEndpoint,
                 DeviceState &state)
{
    wifiManager.addParameter(&customDataEndpoint);

    wifiManager.setAPCallback([&display, &state](WiFiManager *mgr)
                              {
        if (state.displayOk) {
            display.clearDisplay();
            display.setCursor(0, 0);
            display.println("Connect to access point:");
            display.println("AutoConnectAP");
            display.display();
        } });

    if (state.displayOk)
    {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("Connecting to ");
        display.println(WiFi.SSID());
        display.display();
    }

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
