#include <avr/sleep.h>
#include <avr/interrupt.h>

volatile bool wakeFlag = false;

#define BUTTON_PIN 2   // INT0
#define POT_PIN A0     // Potentiometer
#define LED_PIN 13     // External LED

void setup() {
  Serial.begin(9600);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Button with pull-up

  // Interrupt on falling edge (button press)
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), wakeUpISR, FALLING);

  Serial.println("Entering sleep...");
}

void loop() {

  wakeFlag = false;

  // Go to Power-Down sleep
  enterSleep();

  // ----- WOKE UP HERE -----
  int adcValue = analogRead(POT_PIN);
  float voltage = adcValue * (5.0 / 1023.0);

  Serial.print("ADC: ");
  Serial.print(adcValue);
  Serial.print("  Voltage: ");
  Serial.print(voltage, 3);
  Serial.println(" V");

  // LED ON if potentiometer value is high
  if (adcValue > 512) digitalWrite(LED_PIN, HIGH);
  else digitalWrite(LED_PIN, LOW);

  delay(300);               // Short activity time
  digitalWrite(LED_PIN, LOW); // Turn LED off before sleeping again
}

// ===== Interrupt Handler =====
void wakeUpISR() {
  wakeFlag = true;
}

// ===== Enter Power-Down Sleep =====
void enterSleep() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  noInterrupts();
  EIFR = bit(INTF0); // Clear interrupt flag
  interrupts();

  sleep_cpu();       // MCU sleeps here

  sleep_disable();   // Wakes up here
}
