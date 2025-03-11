#include <Wire.h>

void setup() {
  Wire.begin(); // Inizializza il bus I2C
  Serial.begin(9600); // Avvia la comunicazione seriale
  while (!Serial) {
    ; // Aspetta che la connessione seriale sia stabilita
  }
  Serial.println("Scansione I2C in corso...");
}

void loop() {
  int nDevices = 0;

  for (byte address = 1; address < 127; address++) { // Indirizzi I2C validi: 1-127
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Dispositivo trovato all'indirizzo 0x");
      if (address < 16)
        Serial.print("0"); // Aggiungi lo zero iniziale per formattazione
      Serial.println(address, HEX);

      nDevices++;
    } else if (error == 4) {
      Serial.print("Errore sconosciuto all'indirizzo 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  if (nDevices == 0) {
    Serial.println("Nessun dispositivo trovato.");
  } else {
    Serial.println("Scansione completata.");
  }

  delay(5000); // Aspetta 5 secondi prima di una nuova scansione
}
