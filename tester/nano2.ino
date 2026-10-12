
#include <Wire.h>

const byte I2C_ADDRESS = 0x09;

const byte buttonPin = 2;
const byte switchA = 3;
const byte switchB = 4;

volatile byte inputStates = 0;

byte readSwitch() {
  bool a = digitalRead(switchA) == LOW;
  bool b = digitalRead(switchB) == LOW;

  if (a && !b) return 1;  // Side A
  if (!a && !b) return 0; // Center
  if (!a && b) return 2;  // Side B
  return 3;               // Unexpected state
}

void sendInputStates() {
  Wire.write(inputStates);
}

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(switchA, INPUT_PULLUP);
  pinMode(switchB, INPUT_PULLUP);

  Wire.begin(I2C_ADDRESS);
  Wire.onRequest(sendInputStates);
}

void loop() {
  byte states = 0;

  // Button: reversed to match your observed behavior
  if (digitalRead(buttonPin) == HIGH) {
    states |= (1 << 0);
  }

  // Switch position is encoded in bits 1-2
  states |= (readSwitch() << 1);

  noInterrupts();
  inputStates = states;
  interrupts();

  delay(5);
}
