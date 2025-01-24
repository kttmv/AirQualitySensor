#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SHT4x.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
#include <DNSServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <MHZ19.h>
#include <SoftwareSerial.h>
#include <WiFiManager.h>
#include <Wire.h>

// Hardware configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
#define RX_PIN 3
#define TX_PIN 1
#define SDA_PIN 0
#define SCL_PIN 2

// Sensor update interval
const unsigned long SENSOR_UPDATE_INTERVAL = 1000;

// Global objects
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SHT4x sht4 = Adafruit_SHT4x();
MHZ19 myMHZ19;
SoftwareSerial mySerial(RX_PIN, TX_PIN);
WiFiServer server(80);
WiFiManager wifiManager;

// Data endpoint
char dataEndpoint[80];  // To store the server address
WiFiManagerParameter customDataEndpoint("dataEndpoint", "Data endpoint",
                                        dataEndpoint, 80);
unsigned long lastPostMillis = 0;
const unsigned long POST_INTERVAL = 5000;

// Buffer for calculating average of sensor readings
const int BUFFER_SIZE = 12;

struct SensorReading {
  float temperature;
  float humidity;
  int co2;
};

struct AveragingBuffer {
  SensorReading readings[BUFFER_SIZE];
  int currentIndex = 0;
  int count = 0;
} buffer;

// Global state
struct DeviceState {
  bool displayOk = false;
  bool sht4Ok = false;
  bool mhz19Ok = false;
  float temperature = 0.0;
  float humidity = 0.0;
  int co2 = 0;
  unsigned long lastUpdate = 0;
} state;

void initDisplay() {
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.beginTransmission(SCREEN_ADDRESS);
  bool devicePresent = (Wire.endTransmission() == 0);

  if (!devicePresent) {
    state.displayOk = false;
    return;
  }

  state.displayOk = display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  if (state.displayOk) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
  }
}

void initSHT4() {
  state.sht4Ok = sht4.begin();
  if (state.sht4Ok) {
    sht4.setPrecision(SHT4X_HIGH_PRECISION);
    sht4.setHeater(SHT4X_NO_HEATER);
  }
}

void initMHZ19() {
  mySerial.begin(9600);
  myMHZ19.begin(mySerial);
  myMHZ19.autoCalibration();
  state.mhz19Ok = true;
}

void connectWiFi() {
  if (state.displayOk) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Connect to access point:");
    display.println("AutoConnectAP");
    display.display();
  }

  wifiManager.addParameter(&customDataEndpoint);
  wifiManager.autoConnect("AutoConnectAP");

  // Copy the server address after connection
  strcpy(dataEndpoint, customDataEndpoint.getValue());

  if (state.displayOk) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("WiFi connected!");
    display.println("IP address:");
    display.println(WiFi.localIP());
    display.println("Data endpoint:");
    display.println(dataEndpoint);
    display.display();
    delay(3000);
  }
}

SensorReading calculateAverages() {
  float tempSum = 0;
  float humSum = 0;
  int co2Sum = 0;

  for (int i = 0; i < buffer.count; i++) {
    tempSum += buffer.readings[i].temperature;
    humSum += buffer.readings[i].humidity;
    co2Sum += buffer.readings[i].co2;
  }

  if (buffer.count > 0) {
    return {tempSum / buffer.count, humSum / buffer.count,
            co2Sum / buffer.count};
  } else {
    return {0, 0, 0};
  }
}

void updateSensors() {
  if (state.sht4Ok) {
    sensors_event_t humidity, temp;
    sht4.getEvent(&humidity, &temp);
    state.temperature = temp.temperature;
    state.humidity = humidity.relative_humidity;
  }

  if (state.mhz19Ok) {
    state.co2 = myMHZ19.getCO2();
  }

  // Store in buffer
  buffer.readings[buffer.currentIndex] = {state.temperature, state.humidity,
                                          state.co2};

  buffer.currentIndex = (buffer.currentIndex + 1) % BUFFER_SIZE;

  if (buffer.count < BUFFER_SIZE) {
    buffer.count++;
  }
}

void updateDisplay() {
  if (!state.displayOk) return;

  display.clearDisplay();
  display.setCursor(0, 0);

  if (state.sht4Ok) {
    display.print("Temp: ");
    display.print(state.temperature, 1);
    display.println(" C");

    display.setCursor(0, 20);
    display.print("Humidity: ");
    display.print(state.humidity, 1);
    display.println("%");
  } else {
    display.println("SHT40 Error");
  }

  display.setCursor(0, 40);
  if (state.mhz19Ok) {
    display.print("CO2: ");
    display.print(state.co2);
    display.println(" ppm");
  } else {
    display.println("MHZ19B Error");
  }

  display.display();
}

String generateHtmlResponse() {
  String html = "<!DOCTYPE HTML><html><head>";
  html +=
      "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html +=
      "<style>body{font-family:Arial;margin:20px;} "
      ".reset-btn{background-color:#ff3333;color:white;padding:10px "
      "20px;border:none;border-radius:4px;cursor:pointer;}</style></"
      "head><body>";

  html += "<h2>Sensor Status</h2>";
  html += "<p>Display: " + String(state.displayOk ? "OK" : "Error") + "</p>";
  html += "<p>SHT40: " + String(state.sht4Ok ? "OK" : "Error") + "</p>";
  html += "<p>MH-Z19B: " + String(state.mhz19Ok ? "OK" : "Error") + "</p>";

  if (state.sht4Ok) {
    html += "<p>Temperature: " + String(state.temperature, 1) + " °C</p>";
    html += "<p>Humidity: " + String(state.humidity, 1) + "%</p>";
  }

  if (state.mhz19Ok) {
    html += "<p>CO2: " + String(state.co2) + " ppm</p>";
  }

  html += "<form method='POST' action='/reset'>";
  html +=
      "<input type='submit' class='reset-btn' value='Reset WiFi "
      "Configuration'>";
  html += "</form>";

  html += "</body></html>";
  return html;
}

void handleClient() {
  WiFiClient client = server.available();
  if (client) {
    String request = client.readStringUntil('\r');
    client.flush();

    // Check if it's a POST request to /reset
    if (request.indexOf("POST /reset") != -1) {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println();
      client.println("<!DOCTYPE HTML><html><head>");
      client.println(
          "<meta name='viewport' content='width=device-width, "
          "initial-scale=1'>");
      client.println("</head><body>");
      client.println("<h2>Resetting WiFi Configuration...</h2>");
      client.println("<p>Device will restart in access point mode.</p>");
      client.println("<p>Connect to 'AutoConnectAP' to reconfigure.</p>");
      client.println("</body></html>");
      client.stop();

      // Display reset message if display is available
      if (state.displayOk) {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("WiFi Reset");
        display.println("Connect to:");
        display.println("AutoConnectAP");
        display.display();
      }

      // Small delay to ensure the response is sent
      delay(1000);

      // Reset WiFi settings
      wifiManager.resetSettings();

      // Restart the ESP
      ESP.restart();
    } else {
      // Regular status page
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("");
      client.println(generateHtmlResponse());
    }
    client.stop();
  }
}

void sendDataToServer() {
  if (WiFi.status() != WL_CONNECTED) return;

  SensorReading averages = calculateAverages();

  WiFiClient client;
  HTTPClient http;
  http.begin(client, dataEndpoint);
  http.addHeader("Content-Type", "application/json");

  StaticJsonDocument<200> doc;
  doc["temperature"] = averages.temperature;
  doc["humidity"] = averages.humidity;
  doc["co2"] = averages.co2;

  String jsonString;
  serializeJson(doc, jsonString);

  int httpResponseCode = http.POST(jsonString);

  if (httpResponseCode > 0 && state.displayOk) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Avg data sent!");
    display.println("Response: " + String(httpResponseCode));
    display.display();
    delay(1000);
  }

  http.end();
}

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);

  initDisplay();
  initSHT4();
  initMHZ19();
  connectWiFi();
  server.begin();
}

void loop() {
  handleClient();

  unsigned long currentMillis = millis();

  // Update sensors
  if (currentMillis - state.lastUpdate >= SENSOR_UPDATE_INTERVAL) {
    state.lastUpdate = currentMillis;
    updateSensors();
    updateDisplay();
  }

  // Send data to server
  if (currentMillis - lastPostMillis >= POST_INTERVAL) {
    lastPostMillis = currentMillis;
    sendDataToServer();
  }
}
