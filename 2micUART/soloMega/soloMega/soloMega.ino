#include <Wire.h>
#include <ArduinoNunchuk.h>
#include <Adafruit_PWMServoDriver.h>  // Libreria per PCA9685

#define BAUDRATE 9600

ArduinoNunchuk nunchuk = ArduinoNunchuk();
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);  // PCA del Mega
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x41);  // PCA del Mega

const int servoChannels[7] = {0, 1, 2, 3, 4, 5, 6};
const int minPulse = 150; 
const int maxPulse = 400;

bool cButtonState = false;
bool lastCButtonState = false;
bool zButtonState = false;
bool lastZButtonState = false;


bool cycleActive = false;
const int openPulse = 400;  // Valore PWM per palpebra aperta
const int closePulse = 100; // Valore PWM per palpebra chiusa
unsigned long lastCycleTime = 0; // Tempo dell'ultimo aggiornamento del ciclo
int currentAngle = openPulse;  // Angolo attuale del ciclo sinusoidale
bool increasing = true;        // Direzione del ciclo sinusoidale

bool onlyCycle = true;

void setup() {
  
  Serial.println("partenza");
  
  Serial.begin(BAUDRATE);    // Debug su monitor seriale
  Serial1.begin(BAUDRATE);   // Comunicazione UART verso il Nano
  Wire.setClock(100000);     // Imposta velocità I²C a 100 kHz
  pwm.begin();
  pwm.setPWMFreq(50);        // Frequenza per i servo

  pwm1.begin();
  pwm1.setPWMFreq(50);        // Frequenza per i servo

  nunchuk.init();
  Serial.println("\nMega pronto. Nunchuk inizializzato.");
}

unsigned long closeLidStartTime = 0;  // Tempo di inizio chiusura
bool isClosingLids = false;          // Stato del ciclo di chiusura

void loop() {
  Serial.println("ciao");
  nunchuk.update();

  // Leggi i pulsanti
  cButtonState = nunchuk.cButton;
  zButtonState = nunchuk.zButton;

  if(onlyCycle)
  {
    Serial.println("ciclo");
    executeSinusoidalCycle();
    return;
  }

  // Attiva/disattiva il ciclo
  if (cButtonState != lastCButtonState) {
    lastCButtonState = cButtonState;
    cycleActive = !cycleActive;
    if(cycleActive)
      Serial.println("CICLO ATTIVATO");
    else
      Serial.println("CICLO DISATTIVATO");
  }

  // Attiva la chiusura non bloccante con Z
  if (zButtonState != lastZButtonState) {
    lastZButtonState = zButtonState;
    if (zButtonState) {
      Serial.println("Z PRESSED");
      startCloseAndOpenLids();  // Inizia la sequenza non bloccante
    }
  }

  // Esegui la sequenza di chiusura/apertura non bloccante
  updateCloseAndOpenLids();

  // Controlla e aggiorna l'asse X
  if (!isClosingLids) {  // Muovi l'asse X solo se non stai chiudendo le palpebre
    updateAxisX();
  }

  // Esegui il ciclo sinusoidale
  if (cycleActive) {
    executeSinusoidalCycle();
  }

  delay(100);
}

void startCloseAndOpenLids() {
  isClosingLids = true;         // Attiva lo stato di chiusura
  closeLidStartTime = millis(); // Imposta il tempo di inizio
}

void updateCloseAndOpenLids() {
  if (!isClosingLids) return;

  unsigned long currentTime = millis();
  if (currentTime - closeLidStartTime < 250) {
    // Prima fase: chiusura
    for (int i = 0; i < 7; i++) {
      pwm1.setPWM(servoChannels[i], 0, closePulse);
    }
  } else if (currentTime - closeLidStartTime < 500) {
    // Seconda fase: apertura
    for (int i = 0; i < 7; i++) {
      pwm1.setPWM(servoChannels[i], 0, openPulse);
    }
  } else {
    // Sequenza completata
    isClosingLids = false;
  }
}

void updateAxisX() {
  int joyX = nunchuk.analogX;
  Serial.println(joyX);
  if(joyX > 50 && joyX < 150)
  {
    int servoPulse = map(joyX, 0, 255, minPulse, maxPulse);
    for (int i = 0; i < 7; i++) {
      pwm.setPWM(servoChannels[i], 0, 0);
    }
  }else{
    int servoPulse = map(joyX, 0, 255, minPulse, maxPulse);
    for (int i = 0; i < 7; i++) {
      pwm.setPWM(servoChannels[i], 0, servoPulse);
    }
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
      //aggiorno palpebre
      pwm.setPWM(servoChannels[i], 0, currentAngle);
      //aggiorno occhi
      pwm1.setPWM(servoChannels[i], 0, currentAngle);
    }
  }
}

void closeAndOpenLids() {
  for (int i = 0; i < 7; i++) {
    pwm1.setPWM(servoChannels[i], 0, openPulse);  // Chiudi le palpebre
  }
  delay(500);  // Mantieni chiuso per un momento
  for (int i = 0; i < 7; i++) {
    pwm1.setPWM(servoChannels[i], 0, closePulse);  // Riapri le palpebre
  }
}



