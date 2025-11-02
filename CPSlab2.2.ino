const int buttonPin = 8;  // Using pin 8 with pull-up
const int ledPin = 13;
volatile bool ledState = false;

ISR(PCINT0_vect) {  // Interrupt vector for PCINT[7:0] (pins 8–13)
  // Check if pin 8 changed from HIGH to LOW
  if (digitalRead(buttonPin) == LOW) {
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
  }
}

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  // Enable pin change interrupt for pin 8
  PCICR |= (1 << PCIE0);     // Enable Pin Change Interrupt for Port B
  PCMSK0 |= (1 << PCINT0);   // Enable interrupt for PB0 (pin 8)
}

void loop() {
  // Nothing here; handled by interrupt
}


