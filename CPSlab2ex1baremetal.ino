#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#define BUTTON_PIN   PD2   // Digital pin 2
#define LED_PIN      PB5   // Digital pin 13

volatile bool ledState = false;

// Interrupt Service Routine for external interrupt INT0
ISR(INT0_vect) {
    ledState = !ledState;
    if (ledState)
        PORTB |= (1 << LED_PIN);   // Turn LED on
    else
        PORTB &= ~(1 << LED_PIN);  // Turn LED off
}

int main(void) {
    // ----- LED setup -----
    DDRB |= (1 << LED_PIN);      // Set PB5 as output

    // ----- Button setup -----
    DDRD &= ~(1 << BUTTON_PIN);  // Set PD2 as input
    PORTD |= (1 << BUTTON_PIN);  // Enable pull-up resistor

    // ----- Interrupt setup -----
    EICRA |= (1 << ISC01);  // INT0 triggered on falling edge (ISC01=1, ISC00=0)
    EICRA &= ~(1 << ISC00);
    EIMSK |= (1 << INT0);   // Enable INT0 interrupt

    sei(); // Enable global interrupts

    while (1) {
        // main loop does nothing, ISR handles LED toggle
    }
}
