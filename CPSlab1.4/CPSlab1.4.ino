#include "sam.h"
#define LED_PIN 20      // D6 → PA20 on MKR1010 (built-in LED)
#define NEW_LED_PIN 10  // D4 → PB10 on MKR1010
#define INPUT_PIN 10    // D2 → PA10

void setup() {
  // Enable the APB clock for the PORT module
  PM->APBBMASK.reg |= PM_APBBMASK_PORT;
  
  // Configure built-in LED_PIN (PA20) as OUTPUT
  PORT->Group[0].DIRSET.reg = (1 << LED_PIN);
  
  // Configure INPUT_PIN (PA10) as INPUT
  PORT->Group[0].DIRCLR.reg = (1 << INPUT_PIN);
  
  // Configure NEW_LED_PIN (PB10 - D4) as OUTPUT
  PORT->Group[1].DIRSET.reg = (1 << NEW_LED_PIN);
}

void loop() {
  // Toggle built-in LED every 500ms
  PORT->Group[0].OUTSET.reg = (1 << LED_PIN);
  delay(500);
  PORT->Group[0].OUTCLR.reg = (1 << LED_PIN);
  delay(500);
  
  // Toggle new LED on PB10 every 1000ms (1 second)
  PORT->Group[1].OUTTGL.reg = (1 << NEW_LED_PIN);
  delay(1000);
}
