#include <Wire.h>
#include "Adafruit_SGP30.h"
#include "rgb_lcd.h"
#include <WiFiNINA.h>
#include <ArduinoJson.h>

rgb_lcd lcd;
Adafruit_SGP30 sgp;

int RelayPin = 7;
int RedPin = 0;
int GreenPin = 1;
int BluePin = 2;

char ssid[] = "iPhonesuperloba";
char pass[] = "fiyelabonita";
const char* host = "metro-students-mobile-apps.onrender.com";
const int httpsPort = 443;

unsigned long lastCallTime = 0;
const unsigned long interval = 5000;
int status = WL_IDLE_STATUS;

void setup() {
  lcd.begin(16, 2);
  pinMode(RelayPin, OUTPUT);
  pinMode(RedPin, OUTPUT);
  pinMode(GreenPin, OUTPUT);
  pinMode(BluePin, OUTPUT);

  // Start the relay in LOW
  digitalWrite(RelayPin, LOW);

  Serial.begin(9600);
  while (!Serial);

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("WiFi module not found!");
    while (true);
  }

  while (status != WL_CONNECTED) {
    Serial.print("Connecting to: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(5000);
  }

  Serial.println("WiFi connected!");
  //Serial.print("IP: ");
  //Serial.println(WiFi.localIP());

  if (!sgp.begin()) {
    Serial.println("SGP30 sensor not found. Check wiring.");
    while (1);
  }

  Serial.println("SGP30 ready.");
  sgp.IAQinit();
}

void loop() {
  if (Serial.available()) {
    delay(100);
    lcd.clear();
    while (Serial.available() > 0) {
      lcd.write(Serial.read());
    }
    delay(1000);
    lcd.clear();
  }

  if (!sgp.IAQmeasure()) {
    Serial.println("Measurement failed");
    lcd.setCursor(0, 0);
    lcd.print("Measurement");
    lcd.setCursor(0, 1);
    lcd.print("Failed!");
    return;
  }

  Serial.print("TVOC: ");
  Serial.print(sgp.TVOC);
  Serial.print(" ppb\t");
  Serial.print("eCO2: ");
  Serial.print(sgp.eCO2);
  Serial.println(" ppm");

  lcd.setCursor(0, 0);
  lcd.print("CO2:");
  if (sgp.eCO2 < 1000) {
    lcd.print(" ");
  }
  lcd.print(sgp.eCO2);
  lcd.print(" ppm   ");

  lcd.setCursor(0, 1);
  if (sgp.eCO2 < 800) {
    lcd.print("Air:Good         ");
    if (!IsPurifierManual())
    { 
      digitalWrite(RelayPin, LOW);
      }else{
        digitalWrite(RelayPin, HIGH);
    }
    digitalWrite(RedPin, LOW);

    digitalWrite(GreenPin, HIGH);
    digitalWrite(BluePin, LOW);
    SetAirQuality("good");
  }
  else if (sgp.eCO2 < 1200) {
    lcd.print("Air:Fair      ");
    digitalWrite(RedPin, LOW);
    digitalWrite(GreenPin, LOW);
    digitalWrite(BluePin, HIGH);
    if (!IsPurifierManual())
    { 
      digitalWrite(RelayPin, LOW);
      }else{
        digitalWrite(RelayPin, HIGH);
    }
  }
  else {
    lcd.print("Air:BAD!      ");
    digitalWrite(RelayPin, HIGH);
    digitalWrite(RedPin, HIGH);
    digitalWrite(GreenPin, LOW);
    digitalWrite(BluePin, LOW);
    SetAirQuality("bad");
  }

  // if (IsPurifierManual()) {
  //   digitalWrite(RelayPin, HIGH);
  // }

  delay(1000);
}

bool IsPurifierManual() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    return false;
  }

  WiFiSSLClient client;
  client.setTimeout(10000);

  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection failed");
    return false;
  }

  // Send HTTP request
  client.println("GET /purifier/status HTTP/1.1");
  client.println("Host: " + String(host));
  client.println("Connection: close");
  client.println();

  // Wait for response
  unsigned long timeout = millis();
  while (!client.available()) {
    if (millis() - timeout > 10000) {
      Serial.println("Client Timeout");
      client.stop();
      return false;
    }
    delay(10);
  }

  // Skip headers
  while (client.available()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {  // End of headers
      break;
    }
  }

  // Read JSON response
  String jsonResponse = "";
  while (client.available()) {
    jsonResponse += (char)client.read();
  }
  client.stop();

  // Debug raw response
  //Serial.print("Raw response: ");
  //Serial.println(jsonResponse);

  // Clean response (remove any non-JSON content)
  int jsonStart = jsonResponse.indexOf('{');
  int jsonEnd = jsonResponse.lastIndexOf('}');
  if (jsonStart == -1 || jsonEnd == -1) {
    //Serial.println("No JSON data found");
    return false;
  }
  String jsonBody = jsonResponse.substring(jsonStart, jsonEnd + 1);

  // Parse JSON
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, jsonBody);
  
  if (error) {
    Serial.print("JSON parse error: ");
    Serial.println(error.c_str());
    Serial.print("JSON body: ");
    Serial.println(jsonBody);
    return false;
  }

  // Validate JSON structure
  if (!doc.containsKey("status")) {
    Serial.println("Missing 'status' field");
    return false;
  }

  const char* status = doc["status"];
  bool isManual = (strcmp(status, "off") == 0);
  //Serial.print("Purifier status: ");
  //Serial.println(status);
  
  return isManual;
}

void SetAirQuality(const char* command) {
  if (WiFi.status() != WL_CONNECTED) return;

  WiFiSSLClient client;
  client.setTimeout(10000);

  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection failed");
    return;
  }

  // Create JSON payload
  StaticJsonDocument<128> jsonDoc;
  jsonDoc["command"] = command;
  String jsonBody;
  serializeJson(jsonDoc, jsonBody);

  // Send HTTP request
  client.println("POST /purifier/quality HTTP/1.1");
  client.println("Host: " + String(host));
  client.println("Content-Type: application/json");
  client.println("Content-Length: " + String(jsonBody.length()));
  client.println("Connection: close");
  client.println();
  client.println(jsonBody);

  // Wait for response with timeout (declare local timeout variable)
  unsigned long postTimeout = millis();
  while (!client.available()) {
    if (millis() - postTimeout > 10000) {
      Serial.println("Client Timeout");
      break;
    }
    delay(10);
  }

   //Read response
  while (client.available()) {
    Serial.write(client.read());
  }
  client.stop();
}