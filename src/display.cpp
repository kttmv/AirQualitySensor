#include "display.h"

void drawHeader(Adafruit_SSD1306 &display, const char *headerText)
{
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println(headerText);
    display.drawLine(0, 16, 128, 16, WHITE);
    display.setTextSize(1);
    display.setCursor(0, 20);
}

void drawFooter(Adafruit_SSD1306 &display, const char *footerText)
{
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
    display.setCursor(0, 0);

    display.print("Temp: ");
    display.print(state.temperature, 1);
    display.println(" C");
    display.setCursor(0, 20);
    display.print("Humidity: ");
    display.print(state.humidity, 1);
    display.println("%");

    display.setCursor(0, 40);

    display.print("CO2: ");
    display.print(state.co2);
    display.println(" ppm");

    display.display();
}
