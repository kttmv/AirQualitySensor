#include "display.h"
#include <Wire.h>

void initDisplay(Adafruit_SSD1306 &display, DeviceState &state)
{
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.beginTransmission(SCREEN_ADDRESS);
    bool devicePresent = (Wire.endTransmission() == 0);

    if (!devicePresent)
    {
        state.displayOk = false;
        return;
    }
    state.displayOk = display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    if (state.displayOk)
    {
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
    }
}

void updateDisplay(Adafruit_SSD1306 &display, const DeviceState &state)
{
    if (!state.displayOk)
        return;

    display.clearDisplay();
    display.setCursor(0, 0);

    if (state.sht4Ok)
    {
        display.print("Temp: ");
        display.print(state.temperature, 1);
        display.println(" C");
        display.setCursor(0, 20);
        display.print("Humidity: ");
        display.print(state.humidity, 1);
        display.println("%");
    }
    else
    {
        display.println("SHT40 Error");
    }

    display.setCursor(0, 40);
    if (state.mhz19Ok)
    {
        display.print("CO2: ");
        display.print(state.co2);
        display.println(" ppm");
    }
    else
    {
        display.println("MHZ19B Error");
    }

    display.display();
}
