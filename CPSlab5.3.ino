// HC-SR04 Distance Reader (Standard Digital Protocol)
// Note: This sensor uses a custom time-based digital protocol (Trig/Echo) 
// to measure distance, NOT the I2C protocol.

// --- Pin Definitions ---
const int TRIG_PIN = 9;   // Digital pin connected to the Trig pin of HC-SR04
const int ECHO_PIN = 8;   // Digital pin connected to the Echo pin of HC-SR04

// --- Measurement Variables ---
long durationMicroseconds;
long distanceCm;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT); // Set Trig pin as output (to send sound pulse)
  pinMode(ECHO_PIN, INPUT);  // Set Echo pin as input (to listen for sound return)
  Serial.println("HC-SR04 Distance Reader Ready.");
  Serial.println("---------------------------------");
}

void loop() {
  // 1. CLEAR the Trig pin: ensure a clean start by sending a LOW pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  
  // 2. TRIGGER the sound pulse: set the Trig pin HIGH for 10 µs
  // (as required in the lab document)
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW); // End the trigger pulse
  
  // 3. MEASURE the duration of the echo pulse
  // pulseIn waits for the Echo pin to go HIGH, starts timing, and stops timing 
  // when the pin goes LOW again. Returns time in microseconds.
  durationMicroseconds = pulseIn(ECHO_PIN, HIGH);
  
  // 4. CALCULATE the distance (cm)
  // Distance = (Time) * (Speed of Sound) / 2
  // Speed of sound is approx 340 m/s = 0.034 cm/µs.
  // The pulse travels out and back, so divide by 2.
  // Simplified conversion: Distance (cm) = Duration / 58
  distanceCm = durationMicroseconds / 58;
  
  // 5. PRINT the result
  Serial.print("Distance: ");
  Serial.print(distanceCm);
  Serial.println(" cm");
  
  // Read distance once every 500ms
  delay(500); 
}