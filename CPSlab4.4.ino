#include <avr/io.h>

#define LED_PIN PB1

int main(void) {
    DDRB |= (1 << LED_PIN);

    // ----- Timer1 setup: 1 second tick -----
    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1B |= (1 << WGM12);               // CTC mode
    TCCR1B |= (1 << CS12) | (1 << CS10);  // prescaler 1024
    OCR1A = 15624;                        // 1 second , we used timer1 for both off and on state where timer keeps counting and the logic occured when the timmer triggered. 
    TCNT1 = 0; 
    TIFR1 |= (1 << OCF1A);

    uint8_t timer = 0;
    uint8_t led_on = 1;

    PORTB |= (1 << LED_PIN);  // Start with LED ON

    while (1) {

        // 1-second event
        if (TIFR1 & (1 << OCF1A)) {
            TIFR1 |= (1 << OCF1A); // clear flag
            timer++;

            if (led_on && timer >= 3) {
                // Turn OFF after 3 seconds
                PORTB &= ~(1 << LED_PIN);
                led_on = 0;
                timer = 0;
            }
            else if (!led_on && timer >= 10) {
                // Turn ON after 10 seconds
                PORTB |= (1 << LED_PIN);
                led_on = 1;
                timer = 0;
            }
        }
    }
}
