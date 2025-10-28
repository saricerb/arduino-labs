int dustPin = A2;
int ledPower = 6;

int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;

float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

void setup(){
  Serial.begin(9600);
  pinMode(ledPower, OUTPUT);
}

void loop(){
  digitalWrite(ledPower, LOW); // Turn on LED
  delayMicroseconds(samplingTime);
  
  voMeasured = analogRead(dustPin); // Read dust value
  
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower, HIGH); // Turn off LED
  delayMicroseconds(sleepTime);
  
  // Convert to voltage
  calcVoltage = voMeasured * (5.0 / 1024);
  
  // Linear equation from Sharp datasheet
  dustDensity = 0.17 * calcVoltage - 0.1;
  
  // Ensure non-negative values
  dustDensity = max(0, dustDensity);
  
  Serial.print("Raw Signal Value (0-1023): ");
  Serial.print(voMeasured);
  
  Serial.print(" - Voltage: ");
  Serial.print(calcVoltage);
  
  Serial.print(" - Dust Density: ");
  Serial.print(dustDensity);
  Serial.println(" mg/m³");
  
  delay(1000);
}