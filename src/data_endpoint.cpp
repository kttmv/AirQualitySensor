#include "data_endpoint.h"

#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>

#include "sensors.h"
#include "display.h"

const unsigned int EEPROM_SIZE = 512;
const unsigned int ENDPOINT_ADDRESS = 0;
const unsigned int MAX_ENDPOINT_LENGTH = 100;

String dataEndpoint = "";

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
}

void sendDataToServer()
{
    if (WiFi.status() != WL_CONNECTED)
        return;

    SensorReading averages = calculateAverages();

    WiFiClient client;
    HTTPClient http;
    http.begin(client, dataEndpoint);
    http.addHeader("Content-Type", "application/json");

    JsonDocument doc;
    doc["temperature"] = averages.temperature;
    doc["humidity"] = averages.humidity;
    doc["co2"] = averages.co2;

    String jsonString;
    serializeJson(doc, jsonString);

    int httpResponseCode = http.POST(jsonString);

    if (httpResponseCode > 0)
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
