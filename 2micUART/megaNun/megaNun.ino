#include <Wire.h>
#include <Adafruit_PWMServoDriver.h> // Libreria per PCA9685
#include <avr/wdt.h>                // Libreria per il Watchdog Timer

#define BAUDRATE 9600
#define LED_PIN 13

/*
* NUMERAZIONE OCCHI
* 1   2
* 5 4 3 
* 6   7
*
* COLLEGAMENTO OCCHI -> CANALI PCA
*  2 -> 3
*  0 -> 2
*  5 -> 4
*/


Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40); // PCA del Mega

const int servoChannels[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9 ,10 ,11, 12, 13, 14, 15};
const int minPulse = 150;
const int maxPulse = 400;

bool sinusoidalActive = false;  // Stato del ciclo sinusoidale
unsigned long lastCycleTime = 0; // Tempo dell'ultimo aggiornamento del ciclo
int currentAngle = 250;  // Angolo attuale del ciclo sinusoidale
bool increasing = true;        // Direzione del ciclo sinusoidale

void setup() {
  Serial3.begin(BAUDRATE);  // Comunicazione con ESP1
  Serial1.begin(BAUDRATE); // Comunicazione con ESP2
  Serial.begin(BAUDRATE);
  Wire.setClock(100000);    // Imposta velocità I²C a 100 kHz

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // LED spento all'avvio

  pwm.begin();
  pwm.setPWMFreq(50); // Frequenza per i servo

  // Configura il Watchdog Timer (8 secondi massimo)
  wdt_enable(WDTO_8S); // Riavvio se il sistema non risponde entro 8 secondi
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
  delay(1000);
  Serial.println("\nMega pronto.");
}

void loop() {
  wdt_reset(); // Azzeriamo il Watchdog per evitare riavvii non necessari

  // Leggi i dati ricevuti via UART dall'ESP1
  if (Serial3.available()) {
    String receivedData = Serial3.readStringUntil('\n');
    processESP1Data(receivedData);
    Serial1.println(receivedData);
  }

  // Esegui il ciclo sinusoidale se attivo
  if (sinusoidalActive) {
    movementLeftRight();
    //executeSinusoidalCycle();
  }
  delay(50);
}

void processESP1Data(const String &data) {
  // Esempio di formato: "X,C,Z"
  int comma1 = data.indexOf(',');
  int comma2 = data.indexOf(',', comma1 + 1);
  String xValue = ""; 

  if (comma1 > 0 && comma2 > comma1) {
    xValue = data.substring(0, comma1); // Valore X
    String cButtonState = data.substring(comma1 + 1, comma2);
    String zButtonState = data.substring(comma2 + 1);

    // Inoltra i comandi relativi a Z e C all'ESP2
    if (cButtonState == "1") {
      Serial1.println("C:PRESSED");
      sinusoidalActive = !sinusoidalActive; // Toggle del ciclo sinusoidale
    }
    if (zButtonState == "1") {
      Serial1.println("Z:PRESSED");
    }
  }

  int joyX = xValue.toInt();
  Serial.println(joyX);
  if (joyX > 50 && joyX < 150) {
    for (int i = 0; i < 16; i++) {
      pwm.setPWM(servoChannels[i], 0, 0);  // Fermo
    }
  } else {
    int servoPulse = map(joyX, 0, 255, minPulse, maxPulse);
    for (int i = 0; i < 16; i++) {
      pwm.setPWM(servoChannels[i], 0, servoPulse);
    }
  }
}

/*

const int openPulse = 250;  // Valore PWM per palpebra aperta
const int closePulse = 400; // Valore PWM per palpebra chiusa

void executeSinusoidalCycle() {
  unsigned long currentTime = millis();
  int pulseStep = 5;      // Incremento per ogni passo
  int delayTime = 100;    // Ritardo tra ogni passo (aumentato per stabilità)

  if (currentTime - lastCycleTime >= delayTime) {
    lastCycleTime = currentTime;

    if (increasing) {
      currentAngle += pulseStep;
      if (currentAngle >= closePulse) {
        currentAngle = closePulse;
        increasing = false;
      }
    } else {
      currentAngle -= pulseStep;
      if (currentAngle <= openPulse) {
        currentAngle = openPulse;
        increasing = true;
      }
    }

    for (int i = 0; i < 16; i++) {
      pwm.setPWM(servoChannels[i], 0, currentAngle);
    }
  }
}
*/
void movementLeftRight() {
  static int X = 20;
  static int direction = 1;      // Direzione del movimento
  static unsigned long lastCycleTime = 0;
  static unsigned long pauseTime = 0;  // Durata della pausa
  static bool isPaused = false;

  unsigned long currentTime = millis();

  if (isPaused) {
    if (currentTime - lastCycleTime >= pauseTime) {
      isPaused = false;  // Riprendi il movimento
    }
    return;
  }

  if (currentTime - lastCycleTime >= 50) {  // Aggiorna ogni 50 ms
    lastCycleTime = currentTime;

    X += 5 * direction;  // Cambia la posizione
    if (X >= 170 || X <= 20) {
      direction *= -1;
      isPaused = true;  // Pausa quando raggiungi un estremo
      pauseTime = random(500, 1500);  // Durata della pausa
    }

    int servoPulse = map(X, 0, 255, minPulse, maxPulse);
    for (int i = 0; i < 16; i++) {
      pwm.setPWM(servoChannels[i], 0, servoPulse);
    }
  }
}

