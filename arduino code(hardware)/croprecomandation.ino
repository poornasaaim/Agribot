#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define COLOR_SENSOR_PIN_OUT 19 // Pin connected to OUT of color sensor
#define TDS_SENSOR_PIN 18       // Pin connected to TDS sensor
#define TEMP_SENSOR_PIN 12      // Pin connected to temperature sensor
#define PH_SENSOR_PIN 14      // Pin connected to soil pH sensor


const char* ssid = "hotspot";
const char* password = "123456789";
const char* apiKey = "BVF6NNX9B7X4T6Rp";
const char* server = "api.thingspeak.com";

WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Connect to WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected to WiFi");

}

void loop() {
  // Read sensor values
  int colorValue = analogRead(COLOR_SENSOR_PIN_OUT);
  int tdsValue = analogRead(TDS_SENSOR_PIN);
  int tempValue = analogRead(TEMP_SENSOR_PIN);
  int phValue = analogRead(PH_SENSOR_PIN);

  // Create the URL for the ThingSpeak update
  String url = "/update?api_key=";
  url += apiKey;
  url += "&field1=";
  url += String(colorValue);
  url += "&field2=";
  url += String(tdsValue);
  url += "&field3=";
  url += String(tempValue);
  url += "&field4=";
  url += String(phValue);

  // Send the update to ThingSpeak
  if (client.connect(server, 80)) {
    client.print("GET ");
    client.print(url);
    client.println(" HTTP/1.1");
    client.println("Host: api.thingspeak.com");
    client.println("Connection: close");
    client.println();
    delay(1000); // Wait for the server to respond
    client.stop(); // Disconnect from the server
  } else {
    Serial.println("Failed to connect to ThingSpeak");
  }

  // Wait before sending the next update
  delay(15000); // 15 seconds
}
