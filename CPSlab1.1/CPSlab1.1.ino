int ledPin = 6;               /* Declare variable for LED connected to digital pin 6 */
int potentioMeter = A1;       /* Declare variable for potentiometer connected to analog pin A1 */
int val = 0;                  /* Variable to store the analog reading value from potentiometer */

void setup() {
  Serial.begin(9600);          /* Initialize serial communication at 9600 baud rate for monitoring */
  pinMode(ledPin, OUTPUT);     /* Set LED pin as output to control the LED */

}

void loop() { 
  /* Read the analog value from potentiometer (0-1023) */
  val = analogRead(potentioMeter);
  /* Print the analog value to Serial Monitor for debugging/monitoring */
  Serial.println(val); 
  /* Wait for 1 second before next reading */
  delay(1000);

  /* Turn the LED ON by setting pin HIGH */
  digitalWrite (ledPin, HIGH); 
  /* Keep LED ON for 500 milliseconds (0.5 seconds) */
  delay(500); 
  /* Turn the LED OFF by setting pin LOW */
  digitalWrite (ledPin, LOW); 
  /* Keep LED OFF for 500 milliseconds (0.5 seconds) */
  delay(500);
}