#ifndef SENSORS_H
#define SENSORS_H

#include <Adafruit_SHT4x.h>
#include <MHZ19.h>

struct SensorReading
{
    float temperature;
    float humidity;
    int co2;
};

struct AveragingBuffer
{
    SensorReading readings[12];
    int currentIndex = 0;
    int count = 0;
};

struct DeviceState
{
    float temperature = 0.0;
    float humidity = 0.0;
    int co2 = 0;
    unsigned long lastUpdate = 0;
};

extern DeviceState state;
extern Adafruit_SHT4x sht4;
extern MHZ19 myMHZ19;

extern AveragingBuffer buffer;


void initSHT4();
void initMHZ19();
void updateSensors();
SensorReading calculateAverages();

#endif
