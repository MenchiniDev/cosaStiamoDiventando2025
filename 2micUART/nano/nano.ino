#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>  // Libreria per PCA9685

#define BAUDRATE 9600

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);  // Indirizzo PCA9685 del Nano

const int servoChannels[7] = {0, 1, 2, 3, 4, 5, 6};  // Canali PCA per i servo
const int openPulse = 250;  // Valore PWM per palpebra aperta
const int closePulse = 400; // Valore PWM per palpebra chiusa

bool cycleActive = false;      // Stato del ciclo sinusoidale
unsigned long lastCycleTime = 0; // Tempo dell'ultimo aggiornamento del ciclo
int currentAngle = openPulse;  // Angolo attuale del ciclo sinusoidale
bool increasing = true;        // Direzione del ciclo sinusoidale

void setup() {
  Serial.begin(BAUDRATE);  // Comunicazione UART con il Mega
  Wire.setClock(100000);   // Imposta velocità I²C a 100 kHz
  pwm.begin();
  pwm.setPWMFreq(50);      // Frequenza per i servo
  Serial.println("Nano pronto. In attesa di comandi dal Mega.");
}

void loop() {
  checkSerialCommands();  // Leggi comandi dalla UART
  if (cycleActive) {
    executeSinusoidalCycle();  // Esegui il ciclo sinusoidale
  }
  delay(10);  // Riduce il carico
}

void checkSerialCommands() {
  static String receivedData = "";

  // Controlla se ci sono dati disponibili sulla UART
  while (Serial.available()) {
    char incomingChar = Serial.read();

    // Controlla per newline come fine messaggio
    if (incomingChar == '\n') {
      receivedData.trim();  // Pulisce la stringa da spazi o \r
      parseSerialCommand(receivedData);  // Analizza il comando ricevuto
      receivedData = "";  // Resetta la stringa per il prossimo comando
    } else {
      receivedData += incomingChar;  // Accumula il messaggio
    }
  }
}

void parseSerialCommand(const String &command) {
  Serial.print("Ricevuto comando: ");
  Serial.println(command);  // Debug: stampa il comando ricevuto
  
  if (command == "C:PRESSED") {
    cycleActive = !cycleActive;  // Attiva/disattiva il ciclo
    Serial.println(cycleActive ? "Ciclo attivato." : "Ciclo disattivato.");
  } else if (command == "Z:PRESSED") {
    Serial.println("Z premuto. Interruzione ciclo e chiusura palpebre.");
    cycleActive = false;  // Interrompi il ciclo
    closeAndOpenLids();   // Chiudi e riapri le palpebre
  }
}

void executeSinusoidalCycle() {
  unsigned long currentTime = millis();
  int pulseStep = 5;      // Incremento per ogni passo
  int delayTime = 70;     // Ritardo tra ogni passo

  // Controlla se è il momento di aggiornare il ciclo
  if (currentTime - lastCycleTime >= delayTime) {
    lastCycleTime = currentTime;

    // Aggiorna l'angolo
    if (increasing) {
      currentAngle += pulseStep;
      if (currentAngle >= closePulse) {
        currentAngle = closePulse;
        increasing = false;  // Cambia direzione
      }
    } else {
      currentAngle -= pulseStep;
      if (currentAngle <= openPulse) {
        currentAngle = openPulse;
        increasing = true;  // Cambia direzione
      }
    }

    // Muovi tutti i servo
    for (int i = 0; i < 7; i++) {
      pwm.setPWM(servoChannels[i], 0, currentAngle);
    }
  }
}

void closeAndOpenLids() {
  for (int i = 0; i < 7; i++) {
    pwm.setPWM(servoChannels[i], 0, closePulse);  // Chiudi le palpebre
  }
  delay(500);  // Mantieni chiuso per un momento
  for (int i = 0; i < 7; i++) {
    pwm.setPWM(servoChannels[i], 0, openPulse);  // Riapri le palpebre
  }
}
