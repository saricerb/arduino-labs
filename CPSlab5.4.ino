#include <avr/io.h>
#include <util/delay.h>

#define TRIG_PIN PD2   // D2
#define ECHO_PIN PD3   // D3

void uart_init() {
    UBRR0H = 0;
    UBRR0L = 103; // 9600 baud @ 16 MHz
    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_send(char c) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void uart_print(const char *s) {
    while (*s) uart_send(*s++);
}

void uart_print_int(long n) {
    char buf[16];
    ltoa(n, buf, 10);
    uart_print(buf);
}

// ==================== ULTRASONIC ====================
long read_distance_cm() {
    unsigned long duration = 0;

    // Trigger pulse
    PORTD &= ~(1 << TRIG_PIN); 
    _delay_us(2);
    PORTD |= (1 << TRIG_PIN);
    _delay_us(10);
    PORTD &= ~(1 << TRIG_PIN);

    // Wait for echo HIGH
    while (!(PIND & (1 << ECHO_PIN)));

    // Measure time while HIGH
    while (PIND & (1 << ECHO_PIN)) {
        duration++;
        _delay_us(1);
    }

    return (duration * 0.034 / 2);
}

void setup() {
    // TRIG output
    DDRD |= (1 << TRIG_PIN);

    // ECHO input
    DDRD &= ~(1 << ECHO_PIN);

    // PWM pin D9 (OC1A)
    DDRB |= (1 << PB1);

    // Timer1 Fast PWM 8-bit, OC1A = non-inverting
    TCCR1A = (1 << WGM10) | (1 << COM1A1);
    TCCR1B = (1 << WGM12) | (1 << CS11);  // prescaler = 8

    uart_init();
}

void loop() {
    long dist = read_distance_cm();

    uart_print("Distance: ");
    uart_print_int(dist);
    uart_print(" cm\r\n");

    // ==================== FAN CONTROL ====================
    if (dist <= 4) {
        OCR1A = 255;   // full speed
    }
    else if (dist <= 20) {
        int pwm = 255 - ((dist - 4) * 255 / 16);
        if (pwm < 0) pwm = 0;
        OCR1A = pwm;
    }
    else {
        OCR1A = 0;     // off
    }

    _delay_ms(2000);
}
