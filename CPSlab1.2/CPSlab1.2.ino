#include <WiFiNINA.h>  // Include the WiFiNINA library for MKR 1010 WiFi functionality

void setup() {
  Serial.begin(9600);  // Initialize serial communication at 9600 baud rate
  while (!Serial) {    // Wait for serial port to connect (needed for native USB)
    ;
  }
}

void loop() {
  // Check if WiFi module is properly connected and functioning
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("WiFi module not found!");
    while (true);  
  }
  
  Serial.println("Scanning available networks...");
  
  
  int numNetworks = WiFi.scanNetworks();
  
  
  if (numNetworks == 0) {
    Serial.println("No networks found!");
  } else {
    
    for (int i = 0; i < numNetworks; i++) {  // Fixed: changed uint8_t to int and fixed variable declaration
      Serial.print("Network: ");
      Serial.print(WiFi.SSID(i));           
      Serial.print(" | Signal Strength: ");
      Serial.print(WiFi.RSSI(i));           
      Serial.print(" dBm | Encryption: ");
      Serial.println(WiFi.encryptionType(i)); 
    }
    
  
    Serial.print("Total networks found: ");
    Serial.println(numNetworks);
  }
  
  
  delay(30000);
}