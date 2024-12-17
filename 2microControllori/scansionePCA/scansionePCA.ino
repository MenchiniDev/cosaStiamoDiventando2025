#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("\nScansione I2C...");
  
  for (byte address = 1; address < 127; address++) {
    Serial.println(address);
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.print("Dispositivo trovato a 0x");
      Serial.println(address, HEX);
    }
  }
  Serial.println("Scansione completata.");
}

void loop() {}
