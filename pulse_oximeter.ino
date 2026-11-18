#include <Wire.h>
#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 dac;

// --- PINS ---
const int PIN_RED_DRIVE = 7;
const int PIN_IR_DRIVE  = 8;
const int PIN_SAMPLE    = 4;  // LF398 Logic Pin
const int PIN_SIGNAL    = A0; // LF398 Output

// --- SETTINGS ---
// Adjust this to center your signal (0-4095)
const uint16_t V_REF_VALUE = 2048; 

void setup() {
  Serial.begin(115200);

  pinMode(PIN_RED_DRIVE, OUTPUT);
  pinMode(PIN_IR_DRIVE, OUTPUT);
  pinMode(PIN_SAMPLE, OUTPUT);

  // Default state
  digitalWrite(PIN_RED_DRIVE, LOW);
  digitalWrite(PIN_IR_DRIVE, LOW);
  digitalWrite(PIN_SAMPLE, LOW); // LOW = HOLD, HIGH = TRACK

  if (!dac.begin(0x62)) {
    Serial.println("Error: DAC not found");
    while (1);
  }
  dac.setVoltage(V_REF_VALUE, false);

  // Handshake
  while (Serial.available() == 0) {}
  while (Serial.available() > 0) Serial.read();
}

void loop() {
  unsigned long currentMillis = millis();
  int redValue = 0;
  int irValue = 0;

  // --- 1. Measure RED ---
  digitalWrite(PIN_RED_DRIVE, HIGH);
  digitalWrite(PIN_IR_DRIVE, LOW);
  
  // Wait for LED to turn on fully
  delayMicroseconds(100); 
  
  // START SAMPLING (LF398 Pin 8 HIGH) -> Track the signal
  digitalWrite(PIN_SAMPLE, HIGH);
  delay(2); // Track for 2ms
  
  // HOLD (LF398 Pin 8 LOW) -> Freeze the voltage!
  digitalWrite(PIN_SAMPLE, LOW);
  
  // Turn off LED (The voltage is safely frozen on the capacitor now)
  digitalWrite(PIN_RED_DRIVE, LOW);
  
  // Read the Frozen Value
  redValue = analogRead(PIN_SIGNAL);


  // --- 2. Measure IR ---
  digitalWrite(PIN_RED_DRIVE, LOW);
  digitalWrite(PIN_IR_DRIVE, HIGH);
  
  delayMicroseconds(100);
  
  // START SAMPLING
  digitalWrite(PIN_SAMPLE, HIGH);
  delay(2);
  
  // HOLD
  digitalWrite(PIN_SAMPLE, LOW);
  
  digitalWrite(PIN_IR_DRIVE, LOW);
  
  irValue = analogRead(PIN_SIGNAL);

  // --- 3. Stream ---
  Serial.print(currentMillis);
  Serial.print(",");
  Serial.print(redValue);
  Serial.print(",");
  Serial.println(irValue);
}