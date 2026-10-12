
#include <Wire.h>

const byte I2C_ADDRESS = 0x09;

const byte buttonPin = 2;
const byte switch1A = 3;
const byte switch1B = 4;
const byte switch2A = 5;
const byte switch2B = 6;

volatile byte inputStates = 0;

// Returns: 0 = center, 1 = side A, 2 = side B, 3 = invalid
byte readSwitch(byte pinA, byte pinB) {
  bool a = digitalRead(pinA) == LOW;
  bool b = digitalRead(pinB) == LOW;

  if (a && !b) return 1;
  if (!a && !b) return 0;
  if (!a && b) return 2;

  return 3;
}

void onRequest() {
  Wire.write((byte)inputStates);
}

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(switch1A, INPUT_PULLUP);
  pinMode(switch1B, INPUT_PULLUP);
  pinMode(switch2A, INPUT_PULLUP);
  pinMode(switch2B, INPUT_PULLUP);

  Wire.begin(I2C_ADDRESS);
  Wire.onRequest(onRequest);
}

void loop() {
  byte states = 0;

  // Bit 0: push button (HIGH = pressed)
  if (digitalRead(buttonPin) == HIGH) {
    states |= (1 << 0);
  }

  // Bits 1-2: switch 1
  states |= (readSwitch(switch1A, switch1B) << 1);

  // Bits 3-4: switch 2
  states |= (readSwitch(switch2A, switch2B) << 3);

  noInterrupts();
  inputStates = states;
  interrupts();

  delay(5);
}
