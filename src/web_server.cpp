#include "web_server.h"

#include <ESP8266WebServer.h>

#include "sensors.h"
#include "web_server.h"
#include "wifi_manager.h"
#include "display.h"
#include "data_endpoint.h"
#include "url_utilities.h"
#include "eeprom_utilities.h"

WiFiServer server(80);

String generateHtmlResponse()
{
    String html = "<!DOCTYPE HTML><html><head>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<style>";
    html += "body{font-family:Arial;margin:20px;}";
    html += ".reset-btn{background-color:#ff3333;color:white;padding:10px 20px;border:none;border-radius:4px;cursor:pointer;}";
    html += ".save-btn{background-color:#4CAF50;color:white;padding:10px 20px;border:none;border-radius:4px;cursor:pointer;}";
    html += ".input-field{width:100%;padding:12px 20px;margin:8px 0;box-sizing:border-box;border:2px solid #ccc;border-radius:4px;}";
    html += ".toggle-btn{background-color:#008CBA;color:white;padding:10px 20px;border:none;border-radius:4px;cursor:pointer;}";
    html += "</style></head><body>";

    html += "<p>Temperature: " + String(sensorsState.temperature, 1) + " °C</p>";
    html += "<p>Humidity: " + String(sensorsState.humidity, 1) + "%</p>";
    html += "<p>CO2: " + String(sensorsState.co2) + " ppm</p>";
    html += "<p style='margin-top: 16px;'>MH-Z19B Autocalibration: ";
    html += (mhz19AutoCalibration ? "ON" : "OFF");
    html += "</p>";

    html += "<form method='POST' action='/save_endpoint'>";
    html += "<p>Data Endpoint:</p>";
    html += "<input type='text' name='endpoint' value='" + dataEndpoint + "' class='input-field'>";
    html += "<input type='submit' class='save-btn' value='Save Endpoint'>";
    html += "</form><br>";

    html += "<form method='POST' action='/toggle_autocalibration'>";
    html += "<input type='submit' class='toggle-btn' value='Toggle Autocalibration'>";
    html += "</form><br>";

    html += "<form method='POST' action='/calibrate_mhz19'>";
    html += "<input type='submit' class='reset-btn' value='Calibrate MH-Z19B (400ppm)'>";
    html += "<p><small>Warning: Only press after sensor has been in fresh air for 20+ minutes</small></p>";
    html += "</form><br>";

    html += "<form method='POST' action='/reset'>";
    html += "<input type='submit' class='reset-btn' value='Reset WiFi Configuration'>";
    html += "</form></body></html>";
    return html;
}

void handleClient()
{
    WiFiClient client = server.accept();
    if (!client)
        return;

    String request = client.readStringUntil('\r');
    client.flush();

    if (request.indexOf("POST /save_endpoint") != -1)
    {
        String postData = "";
        while (client.available())
        {
            char c = client.read();
            postData += c;
        }

        int startPos = postData.indexOf("endpoint=") + 9;
        int endPos = postData.indexOf("&", startPos);
        if (endPos == -1)
            endPos = postData.length();
        String newEndpoint = postData.substring(startPos, endPos);
        newEndpoint = urlDecode(newEndpoint);

        dataEndpoint = newEndpoint;
        saveEndpoint(dataEndpoint);

        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println();
        client.println("<!DOCTYPE HTML><html><head>");
        client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
        client.println("<meta http-equiv='refresh' content='2;url=/'>");
        client.println("</head><body>");
        client.println("<h2>Endpoint Saved Successfully!</h2>");
        client.println("<p>Redirecting...</p>");
        client.println("</body></html>");
    }
    else if (request.indexOf("POST /reset") != -1)
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
    else if (request.indexOf("POST /toggle_autocalibration") != -1)
    {
        bool currentState = mhz19AutoCalibration;
        saveAutoCalibration(!currentState);

        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println();
        client.println("<!DOCTYPE HTML><html><head>");
        client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
        client.println("<meta http-equiv='refresh' content='2;url=/'>");
        client.println("</head><body>");
        client.println("<h2>Autocalibration Setting Updated!</h2>");
        client.println("<p>Redirecting...</p>");
        client.println("</body></html>");
    }
    else if (request.indexOf("POST /calibrate_mhz19") != -1)
    {
        calibrateMHZ19();

        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println();
        client.println("<!DOCTYPE HTML><html><head>");
        client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
        client.println("<meta http-equiv='refresh' content='2;url=/'>");
        client.println("</head><body>");
        client.println("<h2>MH-Z19B Calibration Started!</h2>");
        client.println("<p>Autocalibration has been disabled and calibration initiated.</p>");
        client.println("<p>Redirecting...</p>");
        client.println("</body></html>");
    }
    else
    {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("");
        client.println(generateHtmlResponse());
    }
    client.stop();
}