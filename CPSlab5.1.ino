void setup() {
  Serial.begin(38400);   // Baud rate = 38400, format = 8N1 by default
}

void loop() {
  Serial.println("A");
  delay(2000);           // 2 second delay between transmissions
}
