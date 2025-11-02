#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t ledState = 0;

ISR(PCINT0_vect) {
  // Check if pin 8 (PB0) is LOW
  if (!(PINB & (1 << PINB0))) {
    ledState ^= 1; // Toggle LED
    if (ledState)
      PORTB |= (1 << PORTB5);  // Turn LED on (pin 13 = PB5)
    else
      PORTB &= ~(1 << PORTB5); // Turn LED off
  }
}

int main(void) {
  // --- Pin setup ---
  DDRB |= (1 << DDB5);    // Set PB5 (pin 13) as output
  DDRB &= ~(1 << DDB0);   // Set PB0 (pin 8) as input
  PORTB |= (1 << PORTB0); // Enable pull-up on PB0

  // --- Interrupt setup ---
  PCICR |= (1 << PCIE0);   // Enable pin change interrupt for Port B
  PCMSK0 |= (1 << PCINT0); // Enable interrupt on PB0 (pin 8)
  
  sei(); // Enable global interrupts

  while (1) {
    // Do nothing; waiting for interrupt
  }
}
