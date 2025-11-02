#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t ledState = 0;

// Interrupt Service Routine for Timer1 Compare Match A
ISR(TIMER1_COMPA_vect)
{
    // Toggle LED (pin 13 = PB5)
    ledState ^= 1;                // Toggle variable
    if (ledState)
        PORTB |= (1 << PORTB5);   // LED ON
    else
        PORTB &= ~(1 << PORTB5);  // LED OFF
}

int main(void)
{
    // --------------------------
    // 1. Configure LED pin
    // --------------------------
    DDRB |= (1 << DDB5);   // Set PB5 (digital pin 13) as output

    // --------------------------
    // 2. Configure Timer1
    // --------------------------
    TCCR1A = 0;            // Normal port operation, no PWM
    TCCR1B = 0;            // Reset control register

    // CTC mode (Clear Timer on Compare Match)
    // WGM12 = 1 → Mode 4 (CTC)
    // Prescaler = 1024 → CS12=1, CS10=1
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);

    // Compare value for 1 second interval
    // 16 MHz / 1024 = 15625 counts per second
    OCR1A = 15625;

    // --------------------------
    // 3. Enable interrupt
    // --------------------------
    TIMSK1 = (1 << OCIE1A);  // Enable Timer1 Compare A interrupt

    // --------------------------
    // 4. Enable global interrupts
    // --------------------------
    sei();                   // Set global interrupt flag

    // --------------------------
    // 5. Main loop
    // --------------------------
    while (1)
    {
        // Nothing to do — LED toggling handled in ISR
    }
}
