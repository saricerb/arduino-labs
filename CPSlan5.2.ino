#include <avr/io.h>

void uart_init_7E1();
void uart_tx(uint8_t c);

void setup() {
  uart_init_7E1();     // Initialize UART in 7E1 format
}

void loop() {
  uart_tx('A');        // Send character repeatedly
  delay(200);
}

void uart_init_7E1() {
  // Enable double-speed mode (same as Arduino core)
  UCSR0A = (1 << U2X0);

  // Baud rate = 115200 → UBRR ≈ 16 (for double speed)
  UBRR0H = 0;
  UBRR0L = 16;

  // Enable transmitter and receiver
  UCSR0B = (1 << TXEN0) | (1 << RXEN0);

  // Frame format:
  // - 7 data bits → UCSZ01:0 = 10
  // - Even parity → UPM01:0 = 10
  // - 1 stop bit → USBS0 = 0
  UCSR0C =
    (1 << UPM01) |      // Even parity
    (0 << USBS0) |      // 1 stop bit
    (1 << UCSZ01) |     // 7-bit data
    (0 << UCSZ00);
}

void uart_tx(uint8_t c) {
  // Wait until the data register is empty
  while (!(UCSR0A & (1 << UDRE0)));
  UDR0 = c;             // Send byte
}

