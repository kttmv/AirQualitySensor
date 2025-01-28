#include "sensors.h"

#include <Adafruit_SHT4x.h>
#include <MHZ19.h>
#include <SoftwareSerial.h>

#include "main.h"

DeviceState state;
Adafruit_SHT4x sht4;
MHZ19 myMHZ19;

AveragingBuffer buffer;

void initSHT4()
{
    sht4.begin();

    sht4.setPrecision(SHT4X_HIGH_PRECISION);
    sht4.setHeater(SHT4X_NO_HEATER);
}

void initMHZ19()
{
    mySerial.begin(9600);
    myMHZ19.begin(mySerial);
    myMHZ19.autoCalibration();
}

void updateSensors()
{
    sensors_event_t humidity, temp;
    sht4.getEvent(&humidity, &temp);
    state.temperature = temp.temperature;
    state.humidity = humidity.relative_humidity;

    state.co2 = myMHZ19.getCO2();

    buffer.readings[buffer.currentIndex] = {state.temperature, state.humidity,
                                            state.co2};
    buffer.currentIndex = (buffer.currentIndex + 1) % 12;
    if (buffer.count < 12)
    {
        buffer.count++;
    }
}

SensorReading calculateAverages()
{
    float tempSum = 0;
    float humSum = 0;
    int co2Sum = 0;

    for (int i = 0; i < buffer.count; i++)
    {
        tempSum += buffer.readings[i].temperature;
        humSum += buffer.readings[i].humidity;
        co2Sum += buffer.readings[i].co2;
    }

    if (buffer.count > 0)
    {
        return {tempSum / buffer.count, humSum / buffer.count, co2Sum / buffer.count};
    }
    else
    {
        return {0, 0, 0};
    }
}
