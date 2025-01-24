#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <Adafruit_SHT4x.h>
#include <MHZ19.h>
#include <SoftwareSerial.h>

// Sensor update interval
const unsigned long SENSOR_UPDATE_INTERVAL = 1000;

// MHZ19 Pins
#define RX_PIN 3
#define TX_PIN 1

// SHT4 Pins
#define SDA_PIN 0
#define SCL_PIN 2

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
    bool displayOk = false;
    bool sht4Ok = false;
    bool mhz19Ok = false;
    float temperature = 0.0;
    float humidity = 0.0;
    int co2 = 0;
    unsigned long lastUpdate = 0;
};

void initSHT4(Adafruit_SHT4x &sht4, DeviceState &state);
void initMHZ19(SoftwareSerial &mySerial, MHZ19 &myMHZ19, DeviceState &state);
void updateSensors(Adafruit_SHT4x &sht4, MHZ19 &myMHZ19,
                   AveragingBuffer &buffer, DeviceState &state);

#endif
