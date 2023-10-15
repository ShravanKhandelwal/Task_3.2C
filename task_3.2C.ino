#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BH1750.h>
#include <WiFiNINA.h>

#define BH1750_ADDR 0x23 // I2C address of the BH-1750 sensor
Adafruit_BH1750 lightSensor;

const char* ssid = "Your_SSID"; // Replace with your WiFi network SSID
const char* password = "Your_PASSWORD"; // Replace with your WiFi network password
const String IFTTT_WEBHOOK_KEY = "Your_IFTTT_WEBHOOK_KEY"; // Replace with your IFTTT webhook key

void setup() {
  Serial.begin(9600);
  delay(1000);

  if (!lightSensor.begin(BH1750_CONTINUOUS_HIGHRES_MODE, BH1750_ADDR)) {
    Serial.println("BH1750 sensor not found. Please check the connection.");
    while (1);
  }

  connectToWiFi();
}

void loop() {
  float lux = lightSensor.readLightLevel();

  if (lux > 1000) {
    sendIFTTTEvent("sunlight_Detected");
  } else {
    sendIFTTTEvent("sunlight_Not_Detected");
  }

  delay(60000); // Delay for 1 minute before reading the light level again
}

void sendIFTTTEvent(const String &eventName) {
  WiFiClient client;

  if (client.connect("maker.ifttt.com", 80)) {
    Serial.println("Connected to IFTTT");

    String url = "/trigger/" + eventName + "/with/key/" + IFTTT_WEBHOOK_KEY;
    client.print(String("POST ") + url + " HTTP/1.1\r\n" +
                 "Host: maker.ifttt.com\r\n" +
                 "User-Agent: Arduino/1.0\r\n" +
                 "Connection: close\r\n\r\n");

    delay(10);

    Serial.println("Sent HTTP request to IFTTT");
  } else {
    Serial.println("Failed to connect to IFTTT");
  }
  client.stop();
}

void connectToWiFi() {
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi module not found.");
    while (true);
  }

  Serial.print("Connecting to WiFi...");
  while (WiFi.begin(ssid, password) != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
}
