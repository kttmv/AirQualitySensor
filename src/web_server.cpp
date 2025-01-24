#include "web_server.h"
#include <Arduino.h>

String generateHtmlResponse(const DeviceState &state)
{ // Internal only
    String html = "<!DOCTYPE HTML><html><head>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<style>body{font-family:Arial;margin:20px;} .reset-btn{background-color:#ff3333;color:white;padding:10px 20px;border:none;border-radius:4px;cursor:pointer;}</style></head><body>";

    html += "<p>Temperature: " + String(state.temperature, 1) + " °C</p>";
    html += "<p>Humidity: " + String(state.humidity, 1) + "%</p>";
    html += "<p>CO2: " + String(state.co2) + " ppm</p>";

    html += "<form method='POST' action='/reset'>";
    html += "<input type='submit' class='reset-btn' value='Reset WiFi Configuration'>";
    html += "</form></body></html>";
    return html;
}

void handleClient(WiFiServer &server, WiFiManager &wifiManager,
                  Adafruit_SSD1306 &display, DeviceState &state)
{
    WiFiClient client = server.available();
    if (!client)
        return;

    String request = client.readStringUntil('\r');
    client.flush();

    if (request.indexOf("POST /reset") != -1)
    {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println();
        client.println("<!DOCTYPE HTML><html><head>");
        client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
        client.println("</head><body>");
        client.println("<h2>Resetting WiFi Configuration...</h2>");
        client.println("<p>Device will restart in access point mode.</p>");
        client.println("<p>Connect to 'AutoConnectAP' to reconfigure.</p>");
        client.println("</body></html>");
        client.stop();

        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("WiFi Reset");
        display.display();

        delay(1000);

        wifiManager.resetSettings();
        ESP.restart();
    }
    else
    {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("");
        client.println(generateHtmlResponse(state));
        client.stop();
    }
}
