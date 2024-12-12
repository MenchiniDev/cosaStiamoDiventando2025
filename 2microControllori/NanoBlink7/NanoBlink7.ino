#include <Wire.h>
#include <ArduinoNunchuk.h>
#include <Adafruit_PWMServoDriver.h>  // Libreria per PCA9685

#define BAUDRATE 19200

ArduinoNunchuk nunchuk = ArduinoNunchuk();
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x42);  // Indirizzo PCA9685 (modifica se necessario)

const int servoChannels[7] = {0, 1, 2, 3, 4, 5, 6};  // Canali PCA per i servo delle palpebre
const int openPulse = 150;  // Valore PWM per palpebra aperta
const int closePulse = 600; // Valore PWM per palpebra chiusa
bool zButtonPrev = false;

void setup() {
  Serial.begin(BAUDRATE);
  Wire.setClock(400000);  // Imposta velocità I²C a 400 kHz
  pwm.begin();
  pwm.setPWMFreq(50);     // Frequenza per i servo
  nunchuk.init();
  Serial.println("Sistema per il controllo delle palpebre inizializzato.");
}

void loop() {
  nunchuk.update();
  bool zButton = nunchuk.zButton;

  if (zButton && !zButtonPrev) {  // Se il pulsante Z viene premuto
    for (int i = 0; i < 7; i++) {
      pwm.setPWM(servoChannels[i], 0, closePulse);  // Chiudi tutte le palpebre
    }
    delay(500);  // Mantieni chiuso per un momento
    for (int i = 0; i < 7; i++) {
      pwm.setPWM(servoChannels[i], 0, openPulse);  // Riapri tutte le palpebre
    }
  }

  zButtonPrev = zButton;
  delay(20);  // Riduce il carico
}
