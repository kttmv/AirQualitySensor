#ifndef DATA_ENDPOINT_H
#define DATA_ENDPOINT_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "sensors.h"

void sendDataToServer(const char *dataEndpoint, const AveragingBuffer &buffer,
                      Adafruit_SSD1306 &display, const DeviceState &state);

#endif
