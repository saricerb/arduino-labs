#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    // --- Setup pin 9 (PB1 = OC1A) for PWM output ---
    DDRB |= (1 << PB1);

    // --- Configure Timer1 for Fast PWM, 8-bit, non-inverting mode ---
    TCCR1A = (1 << COM1A1) | (1 << WGM10);         // Clear OC1A on compare, 8-bit PWM
    TCCR1B = (1 << WGM12)  | (1 << CS11);          // Prescaler = 8 (good PWM freq)

    while (1)
    {
        _delay_ms(15000);   // Wait 15 seconds before starting fan

        // 5-second ramp: each step is 1 second
        OCR1A = 50;    _delay_ms(1000);   // slow
        OCR1A = 100;   _delay_ms(1000);   // medium-slow
        OCR1A = 150;   _delay_ms(1000);   // medium
        OCR1A = 200;   _delay_ms(1000);   // medium-fast
        OCR1A = 255;   _delay_ms(1000);   // full speed

        OCR1A = 0;                     // Turn fan OFF
    }
}
