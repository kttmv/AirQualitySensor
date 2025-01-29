#include <EEPROM.h>
#include <Arduino.h>
#include "main_screen.h"
#include "sensors.h"

const unsigned int EEPROM_SIZE = 512;
const unsigned int ENDPOINT_ADDRESS = 0;
const unsigned int MAX_ENDPOINT_LENGTH = 100;
const unsigned int AUTO_CALIBRATION_ADDRESS = MAX_ENDPOINT_LENGTH + 1;

String dataEndpoint = "";
bool mhz19bAutoCalibration = false;

void loadAutoCalibration()
{
    EEPROM.begin(EEPROM_SIZE);
    mhz19bAutoCalibration = EEPROM.read(AUTO_CALIBRATION_ADDRESS) == 1;
    EEPROM.end();

    myMHZ19.autoCalibration(mhz19bAutoCalibration);
}

void saveAutoCalibration(bool value)
{
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.write(AUTO_CALIBRATION_ADDRESS, value ? 1 : 0);
    EEPROM.commit();
    EEPROM.end();

    mhz19bAutoCalibration = value;
    myMHZ19.autoCalibration(value);

    std::string message = std::string("Autocalibration: ") + std::string(value ? "ON" : "OFF");
    showCustomFooterMessage(message.c_str());
}

void loadEndpoint()
{
    EEPROM.begin(EEPROM_SIZE);
    dataEndpoint = "";
    for (unsigned int i = 0; i < MAX_ENDPOINT_LENGTH; i++)
    {
        char c = EEPROM.read(ENDPOINT_ADDRESS + i);
        if (c == '\0')
            break;
        dataEndpoint += c;
    }
    EEPROM.end();
}

void saveEndpoint(String endpoint)
{
    EEPROM.begin(EEPROM_SIZE);
    for (unsigned int i = 0; i < endpoint.length(); i++)
    {
        EEPROM.write(ENDPOINT_ADDRESS + i, endpoint[i]);
    }
    EEPROM.write(ENDPOINT_ADDRESS + endpoint.length(), '\0');
    EEPROM.commit();
    EEPROM.end();

    showCustomFooterMessage("Data endpoint saved");
}