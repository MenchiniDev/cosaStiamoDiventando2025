#include <Wire.h>
#include <ArduinoNunchuk.h>
#include <Adafruit_PWMServoDriver.h>  // Libreria per PCA9685
#include <avr/wdt.h>  // Libreria per il Watchdog Timer

#define BAUDRATE 9600
#define LED_PIN 13

ArduinoNunchuk nunchuk = ArduinoNunchuk();
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);  // PCA del Mega

const int servoChannels[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9 ,10 ,11, 12, 13, 14, 15};
const int minPulse = 150;
const int maxPulse = 400;
const int openPulse = 250;  // Valore PWM per palpebra aperta
const int closePulse = 400; // Valore PWM per palpebra chiusa

bool cButtonState = false;
bool lastCButtonState = false;
bool zButtonState = false;
bool lastZButtonState = false;
bool sinusoidalActive = false;  // Stato del ciclo sinusoidale
bool pcaConnected = true;      // Stato della connessione al PCA

unsigned long lastCycleTime = 0; // Tempo dell'ultimo aggiornamento del ciclo
int currentAngle = openPulse;  // Angolo attuale del ciclo sinusoidale
bool increasing = true;        // Direzione del ciclo sinusoidale

void setup() {
  Serial.begin(BAUDRATE);    // Debug su monitor seriale
  Serial1.begin(BAUDRATE);
  Wire.setClock(50000);     // Imposta velocità I²C a 100 kHz

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  // LED spento all'avvio

  pwm.begin();
  pwm.setPWMFreq(50);        // Frequenza per i servo
  nunchuk.init();

  delay(2000);

  Wire.beginTransmission(0x40);
  byte error = Wire.endTransmission();
  if (error != 0) {
    digitalWrite(LED_PIN, HIGH);
    resetPCA9685();
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  Serial.println("\nMega pronto. Nunchuk inizializzato.");

  // Configura il Watchdog Timer (8 secondi massimo)
  wdt_enable(WDTO_8S);  // Riavvio se il sistema non risponde entro 8 secondi
}

void loop() {
  // Azzeriamo il Watchdog per evitare riavvii non necessari
  wdt_reset();

  nunchuk.update();

  // Leggi i pulsanti
  cButtonState = nunchuk.cButton;
  zButtonState = nunchuk.zButton;

  checkI2C();

  // Gestione del pulsante C
  if (cButtonState && !lastCButtonState) {
    sinusoidalActive = !sinusoidalActive;  // Toggle del ciclo sinusoidale
    Serial1.println("C:PRESSED");
  }
  lastCButtonState = cButtonState;

  // Gestione del pulsante Z
  if (zButtonState && !lastZButtonState) {
    Serial1.println(zButtonState ? "Z:PRESSED" : "Z:RELEASED");
  }
  lastZButtonState = zButtonState;

  // Controllo joystick o ciclo sinusoidale
  if (sinusoidalActive) {
    executeSinusoidalCycle();
  } else {
    controlServoWithJoystick();
  }

  delay(50);  // Piccolo ritardo per stabilità del loop
}

void controlServoWithJoystick() {
  int joyX = nunchuk.analogX;

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

    static int lastSentAngle = -1;
    if (currentAngle != lastSentAngle) {
      for (int i = 0; i < 16; i++) {
        pwm.setPWM(servoChannels[i], 0, currentAngle);
      }
      lastSentAngle = currentAngle;
    }
  }
}

void resetPCA9685() {
  pwm.reset();  // Reset hardware del PCA9685
  pwm.begin();
  pwm.setPWMFreq(50);
  delay(200);
  digitalWrite(LED_PIN, HIGH);
}

void checkI2C() {
  Wire.beginTransmission(0x40);
  byte error = Wire.endTransmission();
  if (error != 0) {
    digitalWrite(LED_PIN, LOW);
    resetPCA9685();
  }
}
