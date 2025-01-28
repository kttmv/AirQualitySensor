#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_SSD1306.h>

extern Adafruit_SSD1306 display;

enum class TextAlign
{
    LEFT,
    CENTER,
    RIGHT
};

void initDisplay();
int calculateTextX(const char *text, int textSize, TextAlign alignment);
void drawHeader(const char *headerText, TextAlign alignment = TextAlign::LEFT);
void drawFooter(const char *footerText, TextAlign alignment = TextAlign::CENTER);

#endif
