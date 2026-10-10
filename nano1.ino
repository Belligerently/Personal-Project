
#include <Wire.h>

const byte I2C_ADDRESS = 0x08;
const byte buttons[] = {2, 3, 4, 5};
volatile byte buttonStates = 0;

void sendButtonStates() {
  Wire.write(buttonStates);
}

void setup() {
  for (byte i = 0; i < 4; i++) {
    pinMode(buttons[i], INPUT_PULLUP);
  }

  Wire.begin(I2C_ADDRESS);
  Wire.onRequest(sendButtonStates);
}

void loop() {
  byte states = 0;

  for (byte i = 0; i < 4; i++) {
    if (digitalRead(buttons[i]) == LOW) {
      states |= (1 << i);
    }
  }

  noInterrupts();
  buttonStates = states;
  interrupts();

  delay(5);
}
