
#include <Wire.h>

byte previousNano1 = 255;
byte previousNano2 = 255;

bool readNano(byte address, byte &result) {
  Wire.requestFrom(address, (byte)1);

  if (Wire.available()) {
    result = Wire.read();

    while (Wire.available()) {
      Wire.read();
    }

    return true;
  }

  return false;
}

void printNano1(byte states) {
  Serial.println("=== Nano 1 ===");

  for (byte i = 0; i < 4; i++) {
    Serial.print("Button ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println((states & (1 << i)) ? "PRESSED" : "released");
  }
}

void printNano2(byte states) {
  Serial.println("=== Nano 2 ===");

  Serial.print("Button 1: ");
  Serial.println((states & 1) ? "PRESSED" : "released");

  byte switchPosition = (states >> 1) & 3;

  Serial.print("3-position switch: ");

  switch (switchPosition) {
    case 0:
      Serial.println("CENTER");
      break;
    case 1:
      Serial.println("SIDE A");
      break;
    case 2:
      Serial.println("SIDE B");
      break;
    default:
      Serial.println("INVALID STATE");
      break;
  }
}

void setup() {
  Serial.begin(9600);
  Wire.begin();

  Serial.println("================================");
  Serial.println("Micro master - input test");
  Serial.println("Nano 1: 4 buttons (0x08)");
  Serial.println("Nano 2: 1 button + 1 switch (0x09)");
  Serial.println("================================");
}

void loop() {
  byte states;

  // Read Nano 1
  if (readNano(0x08, states)) {
    if (states != previousNano1) {
      printNano1(states);
      previousNano1 = states;
    }
  } else if (previousNano1 != 254) {
    Serial.println("ERROR: Nano 1 not responding!");
    previousNano1 = 254;
  }

  // Read Nano 2
  if (readNano(0x09, states)) {
    if (states != previousNano2) {
      printNano2(states);
      previousNano2 = states;
    }
  } else if (previousNano2 != 254) {
    Serial.println("ERROR: Nano 2 not responding!");
    previousNano2 = 254;
  }

  delay(50);
}
