/*
 * Lab 3 - Sleep modes with Timer1 interrupt wake-up
 * Functional version for Arduino Uno
 * External LED on pin 8
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <util/delay.h>

#define LED_PIN 8   // External LED connected to pin 8 (through 220Ω resistor)

void setup() {
  // Disable unused peripherals to save power
  power_adc_disable();
  power_spi_disable();
  power_twi_disable();
  // DO NOT disable Timer0 if using delay() or Serial

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // --- Configure Timer1 for CTC mode ---
  TCCR1A = 0;                // Normal operation
  TCCR1B = 0;
  TCCR1B |= (1 << WGM12);    // CTC mode

  // 16 MHz / 1024 = 15625 counts per second
  OCR1A = 15624;             // Compare value for 1 second

  TIMSK1 |= (1 << OCIE1A);   // Enable Timer1 compare interrupt
  TCCR1B |= (1 << CS12) | (1 << CS10); // Start Timer1, prescaler 1024

  sei(); // Enable global interrupts

  // Sleep configuration
  set_sleep_mode(SLEEP_MODE_IDLE);  // <--- Important: IDLE mode keeps Timer1 running
  sleep_enable();
}

ISR(TIMER1_COMPA_vect) {
  digitalWrite(LED_PIN, HIGH);
  _delay_ms(100);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  sleep_cpu();   // Sleep until next Timer1 interrupt
}

