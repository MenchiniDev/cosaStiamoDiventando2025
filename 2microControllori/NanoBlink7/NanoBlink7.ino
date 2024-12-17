#include <Wire.h>
#include <ArduinoNunchuk.h>
#include <Adafruit_PWMServoDriver.h>  // Libreria per PCA9685

#define BAUDRATE 9600

ArduinoNunchuk nunchuk = ArduinoNunchuk();
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);  // Indirizzo PCA9685 (modifica se necessario)

const int servoChannels[7] = {0, 1, 2, 3, 4, 5, 6};  // Canali PCA per i servo delle palpebre
const int openPulse = 250;  // Valore PWM per palpebra aperta
const int closePulse = 400; // Valore PWM per palpebra chiusa
bool zButtonPrev = false;
const int minPulse = 250;  // Valore PWM minimo (corrisponde a 0°)
const int maxPulse = 400;  // Valore PWM massimo (corrisponde a 180°)

bool cycleActive = false;  // Stato del ciclo
bool cButtonState = false;  // Stato attuale del pulsante C
bool lastCButtonState = false;  // Stato precedente del pulsante C

void setup() {
  Serial.begin(BAUDRATE);
  Wire.setClock(100000);  // Imposta velocità I²C a 400 kHz
  pwm.begin();
  pwm.setPWMFreq(50);     // Frequenza per i servo
  nunchuk.init();
  Serial.println("Sistema per il controllo delle palpebre inizializzato.");
}

void loop() {
  nunchuk.update();
  bool zButton = nunchuk.zButton;

  cButtonState = nunchuk.cButton;
  if (cButtonState && !lastCButtonState) {  // Rileva il cambio di stato
    cycleActive = !cycleActive;  // Attiva/disattiva il ciclo
    Serial.println(cycleActive ? "Ciclo attivato." : "Ciclo disattivato.");
  }
  lastCButtonState = cButtonState;
  cycleActive = false;
  if (cycleActive) {
    //Serial.print(12);
    executeSinusoidalCycle();  // Funzione non bloccante per il ciclo
  } else {
  if (zButton && !zButtonPrev) {  // Se il pulsante Z viene premuto
    for (int i = 0; i < 7; i++) {
      pwm.setPWM(servoChannels[i], 0, closePulse);  // Chiudi tutte le palpebre
    }
    Serial.println("bottone premuto, palpebra chiusa");
    delay(500);  // Mantieni chiuso per un momento
    for (int i = 0; i < 7; i++) {
      pwm.setPWM(servoChannels[i], 0, openPulse);  // Riapri tutte le palpebre
    }
  }
  }

  zButtonPrev = zButton;
  delay(20);  // Riduce il carico
}


void executeSinusoidalCycle() {
  static int wave = 0;
  static int angle = minPulse;
  static bool increasing = true;
  static unsigned long lastTime = 0;

  int pulseStep = 20;      // Incremento per ogni passo
  int waveLength = 5;      // Numero di oscillazioni "crescenti"
  int delayTime = 70;      // Ritardo tra ogni passo

  // Controlla se è il momento di aggiornare il ciclo
  if (millis() - lastTime >= delayTime) {
    lastTime = millis();

    // Movimento sinusoidale
    if (increasing) {
      angle += pulseStep;
      if (angle >= maxPulse) {
        angle = maxPulse;
        increasing = false;
        wave++;
      }
    } else {
      angle -= pulseStep;
      if (angle <= minPulse) {
        angle = minPulse;
        increasing = true;
        wave++;
      }
    }

    // Muove tutti i servo asse X
    for (int i = 0; i < 7; i++) {
      pwm.setPWM(servoChannels[i], 0, angle);
    }

    // Termina il ciclo se ha completato tutte le onde o se il ciclo è disattivato
    if (!cycleActive) {
      wave = 0;
      increasing = true;
      cycleActive = false;
      Serial.println("Ciclo terminato.");
    }
  }
}

