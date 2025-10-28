#include <Wire.h>
#include "Adafruit_SGP30.h" 
#include "rgb_lcd.h" 
#include <WiFiNINA.h> 
#include <ArduinoHttpClient.h>


rgb_lcd lcd;

Adafruit_SGP30 sgp;

int RelayPin = 7; 
int RedPin = 0; 
int GreenPin = 2; 
int BluePin = 1; 

char ssid[] = "iPhonesuperloba";     
char pass[] = "fiyelabonita";  
char* url = "https://metro-students-mobile-apps.onrender.com";
unsigned long lastCallTime = 0; 
const unsigned long interval = 5000;

int status = WL_IDLE_STATUS; 

WiFiClient client;


void setup() {
  // Set RelayPin as an output pin 

  lcd.begin(16, 2);
  pinMode(RelayPin, OUTPUT); 

  pinMode(RedPin, OUTPUT);
  pinMode(GreenPin, OUTPUT);
  pinMode(BluePin, OUTPUT);

  
  Serial.begin(9600); 
  while (!Serial);  

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("WiFi module not found!");
    while (true); // Stop if WiFi fails
  }

  // Connect to WiFi
  while (status != WL_CONNECTED) {
    Serial.print("Connecting to: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(5000); // Wait 5 sec before retrying
  }

  Serial.println("WiFi connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());


  if (!sgp.begin()) {
    Serial.println("SGP30 sensor not found. Check wiring.");
    while (1); 

    }

  Serial.println("SGP30 ready.");


  sgp.IAQinit();
}


void loop() {
  // Let's turn on the relay...   
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
    digitalWrite(RelayPin, LOW);
    digitalWrite(RedPin, LOW);
    digitalWrite(GreenPin, HIGH);
    digitalWrite(BluePin, LOW); 
    SetAirQuality("good");
  } 
  if (sgp.eCO2 < 1200) {
    lcd.print("Air:Fair      ");
    digitalWrite(RedPin, LOW);
    digitalWrite(GreenPin, LOW);
    digitalWrite(BluePin, HIGH);
  } 
  if (sgp.eCO2 > 1200 || sgp.TVOC > 400) {
    lcd.print("Air:BAD!      ");
    digitalWrite(RelayPin, HIGH); // Relay ON
    digitalWrite(RedPin, HIGH);
    digitalWrite(GreenPin, LOW);
    digitalWrite(BluePin, LOW); 
    SetAirQuality("bad");
  }

if (IsPurifierManual() == true) {
  //disable sensor 
} 
else{
  //enable sensor
}

  delay(1000); 



}   

/*
 * @brief Checks if the purifier is in manual mode or not.
 * @returns true if the purifier is in manual mode, false if it is in automatic mode.
 */
/*
bool IsPurifierManual() {
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient  http;
    http.begin(url + "/purifier/status");
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.GET();
    if(httpCode == 200) {
      String payload = http.getString();
      Serial.println("Response:");
      Serial.println(payload);

      // Parse JSON
      StaticJsonDocument<200> doc;
      deserializeJson(doc, payload);
      const char* status = doc["status"];
      
      if (strcmp(status, "on") == 0) {
        return false;
      } else if (strcmp(status, "off") == 0) {
        return true;
      } else {
        Serial.println("Unknown status");
      }

    } else {
      Serial.printf("HTTP Error: %d\n", httpCode);
    }

    http.end();
  }else{
    Serial.println("No wifi connection...")
  }
  return false; // fallback: standard to automatic if status is unknown.
}

*/

/**
 * @brief Sends a command to set the air quality level of the purifier.
 *
 * This function checks if the WiFi is connected, and if so, it sends a POST
 * request to the purifier's API with the specified command to adjust the air
 * quality. The command is sent as a JSON payload. The function prints the 
 * response or error message to the serial console.
 *
 * @param command A string representing the air quality command to be sent.
 * !!Important: Param command should be either 'good' or 'bad'  
 */

/*

void SetAirQuality(const char* command) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient  http;
    http.begin(String(url) + "/purifier/quality");
    http.addHeader("Content-Type", "application/json");

    // Stel JSON-body samen
    String json = String("{\"command\":\"") + command + "\"}";

    int httpCode = http.POST(json);
    if (httpCode > 0) {
      Serial.printf("POST response code: %d\n", httpCode);
      String response = http.getString();
      Serial.println("Response:");
      Serial.println(response);
    } else {
      Serial.printf("POST failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  } else {
    Serial.println("No WiFi connection...");
  }
}
*/

void SetAirQuality(const char* command) {
  if (WiFi.status() == WL_CONNECTED) {
    HttpClient http(client, url, 80);
    String json = String("{\"command\":\"") + command + "\"}";
    
    http.post("/purifier/quality", "application/json", json);
    Serial.println("POST sent: " + json);
  }
}

bool IsPurifierManual() {
  if (WiFi.status() == WL_CONNECTED) {
    HttpClient http(client, url, 80); // Use ArduinoHttpClient
    http.get("/purifier/status");

    int httpCode = http.responseStatusCode();
    if (httpCode == 200) {
      String payload = http.responseBody();
      Serial.println("Response: " + payload);

      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, payload);
      if (error) {
        Serial.print("JSON error: ");
        Serial.println(error.c_str());
        return false;
      }

      const char* status = doc["status"];
      return (strcmp(status, "off") == 0); // Manual if "off"
    } else {
      Serial.printf("HTTP Error: %d\n", httpCode);
    }
  } else {
    Serial.println("No WiFi connection...");
  }
  return false; // Default to auto mode
}



 