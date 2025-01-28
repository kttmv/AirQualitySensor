#include "display.h"
#include "sensors.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define CHARACTER_WIDTH 6
#define CHARACTER_HEIGHT 8
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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
