
#include <Wire.h>
#include <Keyboard.h>

// =============================================
// KEY MAPPINGS — EDIT THESE TO REMAP CONTROLS
// Use lowercase letters, numbers, or punctuation.
// =============================================

// Nano 1: four buttons
const uint8_t N1_BUTTON_1 = 'q';
const uint8_t N1_BUTTON_2 = 'w';
const uint8_t N1_BUTTON_3 = 'e';
const uint8_t N1_BUTTON_4 = 'r';

// Nano 2: push button
const uint8_t N2_BUTTON_1 = 't';

// Nano 2: switch 1 positions
const uint8_t SW1_SIDE_A = 'y';
const uint8_t SW1_CENTER = 'u';
const uint8_t SW1_SIDE_B = 'i';

// Nano 2: switch 2 positions
const uint8_t SW2_SIDE_A = 'h';
const uint8_t SW2_CENTER = 'j';
const uint8_t SW2_SIDE_B = 'k';

// =============================================

byte nano1States = 0;
byte nano2States = 0;

uint8_t activeKeys[16];
byte activeCount = 0;

bool readNano(byte address, byte &result) {
  byte count = Wire.requestFrom(address, (byte)1);

  if (count != 1 || !Wire.available()) {
    return false;
  }

  result = Wire.read();
  return true;
}

bool containsKey(const uint8_t keys[], byte count, uint8_t key) {
  for (byte i = 0; i < count; i++) {
    if (keys[i] == key) return true;
  }

  return false;
}

void addKey(uint8_t keys[], byte &count, uint8_t key) {
  if (key == 0) return;

  if (!containsKey(keys, count, key) && count < 16) {
    keys[count++] = key;
  }
}

void updateKeyboard() {
  uint8_t desiredKeys[16];
  byte desiredCount = 0;

  // Nano 1: buttons, bits 0-3
  for (byte i = 0; i < 4; i++) {
    if (nano1States & (1 << i)) {
      const uint8_t keys[] = {
        N1_BUTTON_1, N1_BUTTON_2,
        N1_BUTTON_3, N1_BUTTON_4
      };

      addKey(desiredKeys, desiredCount, keys[i]);
    }
  }

  // Nano 2: button, bit 0
  if (nano2States & 1) {
    addKey(desiredKeys, desiredCount, N2_BUTTON_1);
  }

  // Switch 1: bits 1-2
  byte sw1 = (nano2States >> 1) & 3;

  if (sw1 == 0) addKey(desiredKeys, desiredCount, SW1_CENTER);
  if (sw1 == 1) addKey(desiredKeys, desiredCount, SW1_SIDE_A);
  if (sw1 == 2) addKey(desiredKeys, desiredCount, SW1_SIDE_B);

  // Switch 2: bits 3-4
  byte sw2 = (nano2States >> 3) & 3;

  if (sw2 == 0) addKey(desiredKeys, desiredCount, SW2_CENTER);
  if (sw2 == 1) addKey(desiredKeys, desiredCount, SW2_SIDE_A);
  if (sw2 == 2) addKey(desiredKeys, desiredCount, SW2_SIDE_B);

  // Release keys no longer requested
  for (byte i = 0; i < activeCount; i++) {
    if (!containsKey(desiredKeys, desiredCount, activeKeys[i])) {
      Keyboard.release(activeKeys[i]);
    }
  }

  // Press newly requested keys
  for (byte i = 0; i < desiredCount; i++) {
    if (!containsKey(activeKeys, activeCount, desiredKeys[i])) {
      Keyboard.press(desiredKeys[i]);
    }
  }

  // Remember current key set
  activeCount = desiredCount;

  for (byte i = 0; i < activeCount; i++) {
    activeKeys[i] = desiredKeys[i];
  }
}

void setup() {
  Wire.begin();

  // Give the computer time to recognize the keyboard
  delay(1000);
  Keyboard.begin();

  // Start with all inputs released
  nano1States = 0;
  nano2States = 0;
}

void loop() {
  byte states;

  // If a Nano is not responding, treat its controls as released
  if (readNano(0x08, states)) {
    nano1States = states;
  } else {
    nano1States = 0;
  }

  if (readNano(0x09, states)) {
    nano2States = states;
  } else {
    nano2States = 0;
  }

  updateKeyboard();

  delay(10);
}
