#include "display.h"
#include <ESP8266WiFi.h>

void drawHeader(Adafruit_SSD1306 &display, const char *headerText, int textSize)
{
    display.setTextSize(textSize);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println(headerText);
    display.drawLine(0, textSize * 8, 128, textSize * 8, WHITE);

    display.setCursor(0, textSize * 10);
    display.setTextSize(1);
}

void drawFooter(Adafruit_SSD1306 &display, const char *footerText)
{
    display.setTextSize(1);
    display.drawRect(0, 48, 128, 16, WHITE);
    display.setCursor(4, 52);
    display.print(footerText);
}

void initDisplay(Adafruit_SSD1306 &display, DeviceState &state)
{
    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
}

void updateDisplay(Adafruit_SSD1306 &display, const DeviceState &state)
{
    display.clearDisplay();

    drawHeader(display, WiFi.status() == WL_CONNECTED ? "WiFi connected" : "Wi-fi disconnected", 1);

    display.setTextSize(2);
    display.setTextColor(WHITE);

    // Temperature left
    display.setCursor(4, 12);
    display.print(state.temperature, 1);
    display.write(247); // Degree symbol

    // Humidity right
    display.setCursor(88, 12);
    display.print(state.humidity, 0);
    display.print("%");

    // Second row: CO2
    display.setCursor(4, 30);
    display.print(state.co2, 0);
    display.print(" ppm");

    // Footer
    const char *alertMsg = "";
    if (state.humidity < 30)
    {
        alertMsg = "Low humidity";
    }
    else if (state.humidity > 70)
    {
        alertMsg = "High humidity";
    }
    else if (state.co2 > 1000)
    {
        alertMsg = "High CO2";
    }
    else
    {
        alertMsg = "Normal";
    }

    drawFooter(display, alertMsg);

    display.display();
}