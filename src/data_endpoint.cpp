#include "data_endpoint.h"

#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

#include "sensors.h"
#include "display.h"

void sendDataToServer()
{
    if (WiFi.status() != WL_CONNECTED)
        return;

    SensorReading averages = calculateAverages();

    WiFiClient client;
    HTTPClient http;
    http.begin(client, "");
    http.addHeader("Content-Type", "application/json"); // [1]

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
