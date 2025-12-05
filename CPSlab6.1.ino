#include <SPI.h>

#define CS_PIN 11  // Your MKR1010 SS pin

void setup() {
  Serial.begin(9600);

  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);

  SPI.begin();   // Uses MOSI=8, MISO=10, SCK=9 automatically

  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
}

void loop() {
  digitalWrite(CS_PIN, LOW);
  byte received = SPI.transfer(0x55);
  digitalWrite(CS_PIN, HIGH);

  Serial.print("Received: ");
  Serial.println(received, HEX);

  delay(1000);
}
