
#ifndef EEPROM_UTILITIES_H
#define EEPROM_UTILITIES_H

#include <Arduino.h>

extern String dataEndpoint;
extern bool mhz19AutoCalibration;

void loadAutoCalibration();
void saveAutoCalibration(bool value);
void loadEndpoint();
void saveEndpoint(String endpoint);

#endif