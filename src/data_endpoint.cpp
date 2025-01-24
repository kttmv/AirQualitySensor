#include "data_endpoint.h"
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

static SensorReading calculateAverages(const AveragingBuffer &buffer)
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

void sendDataToServer(const char *dataEndpoint, const AveragingBuffer &buffer,
                      Adafruit_SSD1306 &display, const DeviceState &state)
{
    if (WiFi.status() != WL_CONNECTED)
        return;

    SensorReading averages = calculateAverages(buffer);

    WiFiClient client;
    HTTPClient http;
    http.begin(client, dataEndpoint);
    http.addHeader("Content-Type", "application/json"); // [1]

    StaticJsonDocument<200> doc;
    doc["temperature"] = averages.temperature;
    doc["humidity"] = averages.humidity;
    doc["co2"] = averages.co2;

    String jsonString;
    serializeJson(doc, jsonString);

    int httpResponseCode = http.POST(jsonString);

    if (httpResponseCode > 0 && state.displayOk)
    {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Avg data sent!");
        display.print("Response: ");
        display.println(httpResponseCode);
        display.display();
        delay(1000);
    }

    http.end();
}
