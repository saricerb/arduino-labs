/*
   Exercise 3: Cyber-Physical System
   MCP9808 Temperature Sensor + Fan Speed Control
   States:
      <22°C      → OFF
      22–26°C    → LOW
      27–31°C    → MEDIUM
      >31°C      → HIGH
*/

#include <Wire.h>
#include <Adafruit_MCP9808.h>

Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

#define FAN_PIN 9   // PWM pin connected to MOSFET gate

void setup() {
  Serial.begin(9600);
  Wire.begin();

  Serial.println("Initializing MCP9808...");

  // Sensor initialization
  if (!tempsensor.begin(0x18)) {   // default address
    Serial.println("ERROR: MCP9808 NOT FOUND!");
    while (1);
  }

  Serial.println("MCP9808 Detected!");
  Serial.println("Fan Control System Started.");

  pinMode(FAN_PIN, OUTPUT);
  analogWrite(FAN_PIN, 0); // Start with fan OFF
}

void loop() {
  // Read temperature in °C
  float tempC = tempsensor.readTempC();

  Serial.print("Temperature: ");
  Serial.print(tempC);
  Serial.println(" °C");

  int fanPWM = 0;

  // ---------------------------
  //      STATE MACHINE
  // ---------------------------
  if (tempC < 24) {
    fanPWM = 0;         // OFF
  }
  else if (tempC <= 27) {
    fanPWM = 64;        // LOW (≈25% duty)
  }
  else if (tempC <= 30) {
    fanPWM = 128;       // MEDIUM (≈50% duty)
  }
  else {
    fanPWM = 255;       // HIGH (100% duty)
  }

  // Apply fan speed
  analogWrite(FAN_PIN, fanPWM);

  Serial.print("Fan PWM Output: ");
  Serial.println(fanPWM);

  Serial.println("-------------------------");

  delay(1000); // 1-second updates
}
