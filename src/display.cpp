#include "display.h"
#include <ESP8266WiFi.h>
#include "sensors.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define CHARACTER_WIDTH 6
#define CHARACTER_HEIGHT 8
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

static unsigned long lastReadingSwitch = 0;
static int readingIndex = 0;

static unsigned long lastFooterSwitch = 0;
static bool showWiFiText = true;

int calculateTextX(const char *text, int textSize, TextAlign alignment)
{
    int textWidth = strlen(text) * CHARACTER_WIDTH * textSize;

    if (textWidth < 0)
    {
        return 0;
    }

    switch (alignment)
    {
    case TextAlign::LEFT:
        return 0;
    case TextAlign::CENTER:
        return (SCREEN_WIDTH - textWidth) / 2;
    case TextAlign::RIGHT:
        return SCREEN_WIDTH - textWidth;
    default:
        return 0;
    }
}

void drawHeader(const char *headerText, TextAlign alignment)
{
    display.setTextSize(2);

    int xPos = calculateTextX(headerText, 2, alignment);
    display.setCursor(xPos, 0);
    display.println(headerText);

    display.drawLine(0, 16, SCREEN_WIDTH, 16, WHITE);

    display.setTextSize(1);
    display.setCursor(0, 0);
}

void drawFooter(const char *footerText, TextAlign alignment)
{
    display.setTextSize(1);
    display.drawRect(0, 53, SCREEN_WIDTH, 11, WHITE);

    int xPos = calculateTextX(footerText, 1, alignment);
    int xPadding = 0;
    if (alignment == TextAlign::LEFT)
    {
        xPadding = 2;
    }
    else if (alignment == TextAlign::RIGHT)
    {
        xPadding = -2;
    }

    display.setCursor(xPos + xPadding, 55);
    display.print(footerText);

    display.setCursor(0, 0);
}

void initDisplay()
{
    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
}

void updateDisplay()
{
    display.clearDisplay();

    if (millis() - lastReadingSwitch > 2000)
    {
        readingIndex = (readingIndex + 1) % 3;
        lastReadingSwitch = millis();
    }

    const char *readingTypes[3] = {"T \xF7"
                                   "C",
                                   "RH %", "CO2"};
    drawHeader(readingTypes[readingIndex], TextAlign::LEFT);

    const char *status;
    if (readingIndex == 0) // Temperature
    {
        if (sensorsState.temperature < 18)
            status = "LOW";
        else if (sensorsState.temperature > 24)
            status = "HIGH";
        else
            status = "OK";
    }
    else if (readingIndex == 1) // Humidity
    {
        if (sensorsState.humidity < 40)
            status = "LOW";
        else if (sensorsState.humidity > 60)
            status = "HIGH";
        else
            status = "OK";
    }
    else // CO2
    {
        if (sensorsState.co2 > 1000)
            status = "HIGH";
        else
            status = "OK";
    }
    drawHeader(status, TextAlign::RIGHT);

    char readingValue[10];
    if (readingIndex == 0)
    {
        snprintf(readingValue, sizeof(readingValue), "%.1f", sensorsState.temperature);
    }
    else if (readingIndex == 1)
    {
        snprintf(readingValue, sizeof(readingValue), "%.1f", sensorsState.humidity);
    }
    else
    {
        snprintf(readingValue, sizeof(readingValue), "%d", (int)sensorsState.co2);
    }

    display.setTextSize(4);
    int xPos = calculateTextX(readingValue, 4, TextAlign::CENTER);
    display.setCursor(xPos, 21);
    display.print(readingValue);

    bool wifiConnected = (WiFi.status() == WL_CONNECTED);
    if (wifiConnected)
    {
        if (millis() - lastFooterSwitch > 5000)
        {
            showWiFiText = !showWiFiText;
            lastFooterSwitch = millis();
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
