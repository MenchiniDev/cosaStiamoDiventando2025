#include <Wire.h>
#include <ArduinoNunchuk.h>
#include <Adafruit_PWMServoDriver.h>  // Libreria per PCA9685

#define BAUDRATE 9600

ArduinoNunchuk nunchuk = ArduinoNunchuk();
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);  // Indirizzo PCA9685 (modifica se necessario)

const int servoChannels[7] = {0, 1, 2, 3, 4, 5, 6};  // Canali PCA per i servo X
const int minPulse = 150;  // Valore PWM minimo (corrisponde a 0°)
const int maxPulse = 450;  // Valore PWM massimo (corrisponde a 180°)

bool cycleActive = false;  // Stato del ciclo
bool cButtonState = false;  // Stato attuale del pulsante C
bool lastCButtonState = false;  // Stato precedente del pulsante C

void setup() {
  Serial.begin(BAUDRATE);
  Wire.setClock(400000);  // Imposta velocità I²C a 400 kHz
  pwm.begin();
  pwm.setPWMFreq(50);     // Frequenza per i servo
  nunchuk.init();
  Serial.println("Sistema per asse X inizializzato.");
}

void loop() {
  nunchuk.update();

  // Controllo del pulsante C per attivare/disattivare il ciclo
  cButtonState = nunchuk.cButton;
  if (cButtonState && !lastCButtonState) {  // Rileva il cambio di stato
    cycleActive = !cycleActive;  // Attiva/disattiva il ciclo
    Serial.println(cycleActive ? "Ciclo attivato." : "Ciclo disattivato.");
  }
  lastCButtonState = cButtonState;

  // Esegui il ciclo sinusoidale solo se attivo
  if (cycleActive) {
    executeSinusoidalCycle();  // Funzione non bloccante per il ciclo
  } else {
    // Controllo normale del joystick
    int joyX = nunchuk.analogX;  // Leggi posizione joystick X
    Serial.println(joyX);
    int servoPulse = map(joyX, 0, 255, minPulse, maxPulse);
    for (int i = 0; i < 7; i++) {
      pwm.setPWM(servoChannels[i], 0, servoPulse);  // Muovi tutti i servo asse X
    }
  }

  delay(20);  // Riduce il carico
}

void executeSinusoidalCycle() {
  static int wave = 0;
  static int angle = minPulse;
  static bool increasing = true;
  static unsigned long lastTime = 0;

  int pulseStep = 20;      // Incremento per ogni passo
  int waveLength = 5;      // Numero di oscillazioni "crescenti"
  int delayTime = 20;      // Ritardo tra ogni passo

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
