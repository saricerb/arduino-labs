#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// LED externo en pin 13 (PB5)
#define LED_PIN PB5

volatile uint8_t wakeFlag = 0;

void ADC_init(void) {
    // Seleccionar referencia AVcc (REFS0 = 1)
    ADMUX = (1 << REFS0);

    // Seleccionar canal ADC0 (A0) - MUX = 0000
    ADMUX &= ~(0x0F);

    // Habilitar ADC, prescaler 128 → 125 kHz
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t ADC_read(void) {
    ADCSRA |= (1 << ADSC);     // Iniciar conversión
    while (ADCSRA & (1 << ADSC)); // Esperar
    return ADC;
}

void INT0_init(void) {
    // Pin 2 como entrada (INT0)
    DDRD &= ~(1 << PD2);

    // Habilitar pull-up interno
    PORTD |= (1 << PD2);

    // Interrupción por flanco descendente ISC01 = 1, ISC00 = 0
    EICRA = (1 << ISC01);

    // Habilitar INT0
    EIMSK = (1 << INT0);
}

void enter_sleep(void) {
    // Desactivar ADC para ahorro máximo
    ADCSRA &= ~(1 << ADEN);

    // Modo power-down
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();

    sei();          // Habilitar interrupciones globales
    sleep_cpu();    // 💤 DORMIR

    // --- Despierta aquí ---
    sleep_disable();
}

ISR(INT0_vect) {
    wakeFlag = 1;
}

int main(void) {

    // LED externo como salida
    DDRB |= (1 << LED_PIN);

    // Inicializar interrupción externa
    INT0_init();

    // Inicializar ADC
    ADC_init();

    sei();

    while (1) {
        wakeFlag = 0;

        // DORMIR
        enter_sleep();

        // --- MICROCONTROLADOR DESPIERTO ---

        // Reactivar ADC
        ADCSRA |= (1 << ADEN);

        // Leer potenciómetro A0
        uint16_t value = ADC_read();

        // Encender LED según el valor (simple demostración)
        if (value > 512) {
            PORTB |= (1 << LED_PIN);
        } else {
            PORTB &= ~(1 << LED_PIN);
        }

        _delay_ms(300);

        // LED OFF antes de dormir
        PORTB &= ~(1 << LED_PIN);
    }
}
