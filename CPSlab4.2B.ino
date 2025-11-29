#include <avr/io.h>
#include <avr/interrupt.h>

#define FAN_PWM PB1 // pin 9 as output

volatile uint16_t tick_counting = 0;
volatile uint16_t second_counting = 0;
volatile uint16_t fan_on_flag = 0;      
volatile uint16_t fan_counting = 0;   

void timer_zero() {
    TCCR0A = (1 << WGM01);
    TCCR0B = (1 << CS02) | (1 << CS00);
    OCR0A = 249;
    TIMSK0 |= (1 << OCIE0A);
}

void set_adc() {
    ADMUX = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // prescaler 128
}

uint16_t read_adc() {
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}

void setup_pwm() {
    DDRB |= (1 << FAN_PWM);
    TCCR1A = (1 << COM1A1) | (1 << WGM10);
    TCCR1B = (1 << WGM12) | (1 << CS11);
    OCR1A = 0;
}

ISR(TIMER0_COMPA_vect) {
    tick_counting++;
    if (tick_counting >= 62) { // when ISR occurs 62 times, reset
        tick_counting = 0;
        if (!fan_on_flag) {
            second_counting++;
            if (second_counting >= 10) {
                fan_on_flag = 1;
                fan_counting = 0;
            }
        } else {
            fan_counting++;
            if (fan_counting <= 6) {
                uint16_t potValue = read_adc();
                uint8_t pwmValue = (potValue * 255) / 1023;
                OCR1A = pwmValue;
            } else {
                fan_on_flag = 0;
                second_counting = 0;
                OCR1A = 0;
            }
        }
    }
}

int main(void) {
    setup_pwm();
    set_adc();
    timer_zero();
    sei();
    while (1) {
        // main loop empty
    }
}


