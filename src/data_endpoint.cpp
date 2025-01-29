#include "data_endpoint.h"

#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

#include "sensors.h"
#include "display.h"
#include "main_screen.h"
#include "eeprom_utilities.h"

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
        showCustomFooterMessage(("Data sent (" + std::to_string(httpResponseCode) + ")").c_str());
    }

    http.end();
}
