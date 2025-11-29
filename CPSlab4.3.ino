#include <avr/io.h>
#include <avr/interrupt.h>

#define FAN_PWM PB1   // OC1A, Arduino pin 9
#define MIN_PWM 60    // minimum PWM to overcome stall

volatile uint16_t tick_counter = 0;   // counts ~16ms ticks
volatile uint8_t second_counter = 0;  // counts seconds
volatile uint8_t fan_counter = 0;     // counts fan ON seconds
volatile uint8_t fan_on = 0;
volatile uint16_t adc_val = 0;        // potentiometer reading

// ---------------- ADC ----------------
void setup_adc() {
    ADMUX = (1 << REFS0);  //in this section we are selecting the external voltage as refrence voltage for pin A0
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t read_adc() {
    ADCSRA |= (1 << ADSC);                // start conversion which start conversion in register A 
    while (ADCSRA & (1 << ADSC));         // wait for completion 
    return ADC;
}

// ---------------- PWM ----------------
void setup_pwm() {
    DDRB |= (1 << FAN_PWM);                // OC1A as output
    TCCR1A = (1 << COM1A1) | (1 << WGM10); // Fast PWM 8-bit, non-inverting
    TCCR1B = (1 << WGM12) | (1 << CS11);   // prescaler = 8 (~7.8 kHz)
    OCR1A = 0;                              // start with fan OFF
}

// ---------------- Timer0 ----------------
void setup_timer0() {
    TCCR0A = (1 << WGM01);                // CTC mode same
    TCCR0B = (1 << CS02) | (1 << CS00);   // prescaler 1024
    OCR0A = 249;                           // ~16ms tick
    TIMSK0 |= (1 << OCIE0A);               // enable compare interrupt
}

// ---------------- Timer0 ISR ----------------
ISR(TIMER0_COMPA_vect) {
    tick_counter++;
    if (tick_counter >= 62) { // 62 * 16ms ≈ 1 second
        tick_counter = 0;

        // Read potentiometer continuously
        adc_val = read_adc();

        if (!fan_on) {
            second_counter++;
            if (second_counter >= 10) { // 10s OFF
                fan_on = 1;
                fan_counter = 0;
            }
        } else {
            fan_counter++;
            if (fan_counter <= 6) { // 6s ON
                // Map ADC to PWM with minimum
                OCR1A = ((adc_val * (255 - MIN_PWM)) / 1023) + MIN_PWM;
            } else {
                fan_on = 0;
                second_counter = 0;
                OCR1A = 0; // fan OFF
            }
        }
    }
}

// ---------------- Main ----------------
int main(void) {
    setup_pwm();
    setup_timer0();
    setup_adc();
    sei(); // enable interrupts

    while (1) {
        // Main loop empty, everything handled in ISR
    }
}