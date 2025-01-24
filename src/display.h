#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h> // For Adafruit_SSD1306 [1]
#include "sensors.h"          // For DeviceState [1]

// Hardware configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

void initDisplay(Adafruit_SSD1306 &display, DeviceState &state);
void updateDisplay(Adafruit_SSD1306 &display, const DeviceState &state);
void drawHeader(Adafruit_SSD1306 &display, const char *headerText, int textSize);
void drawFooter(Adafruit_SSD1306 &display, const char *footerText);

#endif
