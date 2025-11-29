#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    // --- Configure PB0 (Arduino D8) as output ---
    DDRB |= (1 << PB1);   // Set pin PB0 as output

    while (1)
    {
        _delay_ms(3000);    // Wait 3 seconds
        PORTB |= (1 << PB1);  // Turn LED ON (drive transistor base high)

        _delay_ms(1000);    // Wait 1 second
        PORTB &= ~(1 << PB1); // Turn LED OFF (drive base low)
    }
}
