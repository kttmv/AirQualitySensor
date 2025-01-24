#include "sensors.h"
#include <Adafruit_SHT4x.h>
#include <MHZ19.h>
#include <SoftwareSerial.h>

void initSHT4(Adafruit_SHT4x &sht4, DeviceState &state)
{
    state.sht4Ok = sht4.begin();
    if (state.sht4Ok)
    {
        sht4.setPrecision(SHT4X_HIGH_PRECISION);
        sht4.setHeater(SHT4X_NO_HEATER);
    }
}

void initMHZ19(SoftwareSerial &mySerial, MHZ19 &myMHZ19, DeviceState &state)
{
    mySerial.begin(9600);
    myMHZ19.begin(mySerial);
    myMHZ19.autoCalibration();
    state.mhz19Ok = true;
}

void updateSensors(Adafruit_SHT4x &sht4, MHZ19 &myMHZ19,
                   AveragingBuffer &buffer, DeviceState &state)
{
    if (state.sht4Ok)
    {
        sensors_event_t humidity, temp;
        sht4.getEvent(&humidity, &temp);
        state.temperature = temp.temperature;
        state.humidity = humidity.relative_humidity;
    }
    if (state.mhz19Ok)
    {
        state.co2 = myMHZ19.getCO2();
    }

    buffer.readings[buffer.currentIndex] = {state.temperature, state.humidity,
                                            state.co2};
    buffer.currentIndex = (buffer.currentIndex + 1) % 12;
    if (buffer.count < 12)
    {
        buffer.count++;
    }
}
