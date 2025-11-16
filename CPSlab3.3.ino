/*
 * Lab 3 - Sleep modes with Timer1 interrupt wake-up
 * Timer1 wakes MCU every 1s
 * Timer0 handles 0.1s LED blink
 * External LED on pin 8
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>

#define LED_PIN 8

volatile bool ledBlink = false;

void setup() {
  // Disable unused peripherals to save power
  power_adc_disable();
  power_spi_disable();
  power_twi_disable();
  // Keep Timer0 enabled for LED timing
  // DO NOT disable Timer0 if you need timing

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // -------- Timer1 setup (1s wake-up) --------
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1 << WGM12);        // CTC mode
  OCR1A = 15624;                 // 16MHz / 1024 prescaler → 1 Hz
  TIMSK1 |= (1 << OCIE1A);       // Enable Timer1 compare match interrupt
  TCCR1B |= (1 << CS12) | (1 << CS10); // Start Timer1 with prescaler 1024

  // -------- Timer0 setup (LED blink 0.1s) --------
  TCCR0A = 0;
  TCCR0B = 0;
  TCCR0A |= (1 << WGM01);        // CTC mode
  OCR0A = 156;                   // 16MHz / 1024 ≈ 156 counts for 10ms → we will count 10 overflows for 0.1s
  TIMSK0 |= (1 << OCIE0A);       // Enable Timer0 compare match
  TCCR0B |= (1 << CS02) | (1 << CS00); // Start Timer0 with prescaler 1024
  TIMSK0 &= ~(1 << OCIE0A);      // Disabled initially, enable only for blink

  // Enable global interrupts
  sei();

  // Set sleep mode to IDLE so Timer1 and Timer0 keep running
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
}

// ---------- Timer1 ISR: wake MCU and trigger LED blink ----------
ISR(TIMER1_COMPA_vect) {
  ledBlink = true;
  TCNT0 = 0;             // Reset Timer0 counter
  TIMSK0 |= (1 << OCIE0A); // Enable Timer0 interrupt to start LED blink
  digitalWrite(LED_PIN, HIGH);
}

// ---------- Timer0 ISR: count 0.1s for LED ----------
volatile uint8_t ledCounter = 0;
ISR(TIMER0_COMPA_vect) {
  ledCounter++;
  if (ledCounter >= 10) { // 10 x 10ms ≈ 0.1s
    digitalWrite(LED_PIN, LOW);
    ledCounter = 0;
    TIMSK0 &= ~(1 << OCIE0A); // Disable Timer0 until next blink
  }
}

void loop() {
  sleep_cpu(); // MCU sleeps until next interrupt
}
