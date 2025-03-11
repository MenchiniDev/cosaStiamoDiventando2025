#include <Wire.h>
#include <ArduinoNunchuk.h>

#define BAUDRATE 9600  // Velocità UART per comunicazione con il Mega
#define MAX_RECONNECT_ATTEMPTS 5  // Numero massimo di tentativi di riconnessione
#define RECONNECT_DELAY 500  // Ritardo tra i tentativi di riconnessione in millisecondi

ArduinoNunchuk nunchuk;

int reconnectAttempts = 0;  // Contatore dei tentativi di riconnessione
bool nunchukConnected = false;  // Stato della connessione al Nunchuk

void setup() {
  Wire.begin(4, 5);  // Inizializza I²C con SDA = pin 4 e SCL = pin 5
  Serial.begin(BAUDRATE);  // Comunicazione UART con il Mega
  Wire.setClock(100000);   // Imposta velocità I²C a 100 kHz
  
  nunchukConnected = initializeNunchuk();  // Prova a connettere il Nunchuk

  if (!nunchukConnected) {
    Serial.println("ERROR: NO NUN");  // Invia errore immediato se fallisce al primo tentativo
  }
  delay(1000);
}

void loop() {
  if (!nunchukConnected) {
    // Prova a riconnettere il Nunchuk
    if (reconnectAttempts < MAX_RECONNECT_ATTEMPTS) {
      reconnectAttempts++;
      delay(RECONNECT_DELAY);
      nunchukConnected = initializeNunchuk();
      if (nunchukConnected) {
        Serial.println("INFO: NUNCHUK RECONNECTED");
        reconnectAttempts = 0;  // Reset tentativi
      }
    } else {
      Serial.println("ERROR: NO NUN");  // Notifica errore dopo tentativi falliti
      delay(1000);  // Ritardo per evitare flooding di messaggi di errore
    }
    return;  // Non esegue il loop normale se non connesso
  }

  // Aggiorna i dati dal Nunchuk e li invia al Mega
  nunchuk.update();
  String data = String(nunchuk.analogX) + "," +
                String(nunchuk.cButton ? "1" : "0") + "," +
                String(nunchuk.zButton ? "1" : "0") + "\n";
  Serial.print(data);  // Invia il pacchetto al Mega
  delay(100);  // Ritardo per evitare sovraccarico
}

bool initializeNunchuk() {
  nunchuk.init();  // Inizializza il Nunchuk
  delay(100);  // Attende brevemente per la stabilizzazione
  return true;
}
