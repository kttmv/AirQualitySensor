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

        // Header
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("WiFi");

        // Divider line
        display.drawLine(0, 16, 128, 16, WHITE);

        display.setTextSize(1);
        display.setCursor(0, 20);
        display.println("To configure wi-fi");
        display.setCursor(0, 30);
        display.println("connect to hotspot:");
        display.setCursor(0, 40);
        display.println("> AutoConnectAP");

        // Footer with timeout
        display.drawRect(0, 48, 128, 16, WHITE);
        display.setCursor(4, 52);
        display.print("Timeout: 60s");

        display.display();
    };

    wifiManager.setAPCallback(displayAPCallback);

    if (state.displayOk)
    {
        display.clearDisplay();

        // Header
        display.setTextSize(2);
        display.setCursor(0, 0);
        display.println("WiFi");

        // Divider line
        display.drawLine(0, 16, 128, 16, WHITE);

        display.setTextSize(1);
        display.setCursor(0, 20);
        display.println("Connecting to:");
        display.setCursor(0, 30);
        display.print("> ");
        display.println(WiFi.SSID());

        // Footer
        display.drawRect(0, 48, 128, 16, WHITE);
        display.setCursor(4, 52);
        display.print("Please wait...");

        display.display();
    }

    if (!wifiManager.autoConnect("AutoConnectAP"))
    {
        if (state.displayOk)
        {
            display.clearDisplay();

            // Header
            display.setTextSize(2);
            display.setCursor(0, 0);
            display.println("Error");

            // Divider line
            display.drawLine(0, 16, 128, 16, WHITE);

            display.setTextSize(1);
            display.setCursor(0, 20);
            display.println("Connection failed");
            display.setCursor(0, 30);
            display.println("Timeout reached");

            // Footer
            display.drawRect(0, 48, 128, 16, WHITE);
            display.setCursor(4, 52);
            display.print("No WiFi available");

            display.display();
            delay(2000);
        }
        return;
    }

    strcpy(dataEndpoint, customDataEndpoint.getValue());

    if (state.displayOk)
    {
        display.clearDisplay();

        // Header
        display.setTextSize(2);
        display.setCursor(0, 0);
        display.println("WiFi");

        // Divider line
        display.drawLine(0, 16, 128, 16, WHITE);

        display.setTextSize(1);
        display.setCursor(0, 20);
        display.println("Connected!");
        display.setCursor(0, 30);
        display.print("> ");
        display.println(WiFi.localIP());
        display.setCursor(0, 40);
        display.print("> ");
        display.println(dataEndpoint);

        // Footer
        display.drawRect(0, 48, 128, 16, WHITE);
        display.setCursor(4, 52);
        display.print("Ready");

        display.display();
        delay(3000);
    }
}
