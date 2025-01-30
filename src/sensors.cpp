#include "sensors.h"

#include <Adafruit_SHT4x.h>
#include <MHZ19.h>
#include <SoftwareSerial.h>

#include "main.h"
#include "eeprom_utilities.h"
#include "main_screen.h"

SensorstState sensorsState;
Adafruit_SHT4x sht4;
MHZ19 myMHZ19;

SensorReadingsBuffer buffer;

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

    loadAutoCalibration();
}

void calibrateMHZ19()
{
    saveAutoCalibration(false);
    myMHZ19.calibrate();
    showCustomFooterMessage("MHZ19 calibrated");
}

void updateSensors()
{
    sensors_event_t humidity, temp;
    sht4.getEvent(&humidity, &temp);
    sensorsState.temperature = temp.temperature;
    sensorsState.humidity = humidity.relative_humidity;

    sensorsState.co2 = myMHZ19.getCO2();

    buffer.readings[buffer.currentIndex] = {sensorsState.temperature, sensorsState.humidity,
                                            sensorsState.co2};

    buffer.currentIndex = (buffer.currentIndex + 1) % SENSOR_READINGS_BUFFER_SIZE;

    if (buffer.count < SENSOR_READINGS_BUFFER_SIZE)
    {
        buffer.count++;
    }
}

SensorReading calculateAverages()
{
    float tempSum = 0;
    float humSum = 0;
    int co2Sum = 0;

    for (unsigned int i = 0; i < buffer.count; i++)
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
