#ifndef SENSORS_H
#define SENSORS_H

#include <Adafruit_SHT4x.h>
#include <MHZ19.h>

const unsigned int SENSOR_READINGS_BUFFER_SIZE = 6;

struct SensorReading
{
    float temperature;
    float humidity;
    unsigned int co2;
};

struct SensorReadingsBuffer
{
    SensorReading readings[SENSOR_READINGS_BUFFER_SIZE];
    unsigned int currentIndex = 0;
    unsigned int count = 0;
};

struct SensorstState
{
    float temperature = 0.0;
    float humidity = 0.0;
    unsigned int co2 = 0;
};

extern SensorstState sensorsState;
extern Adafruit_SHT4x sht4;
extern MHZ19 myMHZ19;

extern SensorReadingsBuffer buffer;

void initSHT4();
void initMHZ19();
void updateSensors();
SensorReading calculateAverages();

#endif
