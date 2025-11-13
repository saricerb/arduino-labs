#define LED_PIN 8
void setup() {
  pinMode(LED_PIN, OUTPUT);
}
void loop() {
  digitalWrite(LED_PIN, HIGH);
  delay(100);   // 0.1 s ON
  digitalWrite(LED_PIN, LOW);
  delay(900);   // 0.9 s OFF
}
