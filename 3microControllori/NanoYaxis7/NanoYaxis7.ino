#include <Wire.h>
#include <ArduinoNunchuk.h>
#include <Adafruit_PWMServoDriver.h>  // Libreria per PCA9685

#define BAUDRATE 19200

ArduinoNunchuk nunchuk = ArduinoNunchuk();
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);  // Indirizzo PCA9685 (modifica se necessario)

const int servoChannels[7] = {0, 1, 2, 3, 4, 5, 6};  // Canali PCA per i servo Y
const int minPulse = 150;  // Valore PWM minimo (corrisponde a 0°)
const int maxPulse = 600;  // Valore PWM massimo (corrisponde a 180°)

void setup() {
  Serial.begin(BAUDRATE);
  Wire.setClock(400000);  // Imposta velocità I²C a 400 kHz
  pwm.begin();
  pwm.setPWMFreq(50);     // Frequenza per i servo
  nunchuk.init();
  Serial.println("Sistema per asse Y inizializzato.");
}

void loop() {
  nunchuk.update();
  int joyY = nunchuk.analogY;  // Leggi posizione joystick Y
  int servoPulse = map(joyY, 0, 255, minPulse, maxPulse);

  for (int i = 0; i < 7; i++) {
    pwm.setPWM(servoChannels[i], 0, servoPulse);  // Muovi tutti i servo asse Y
  }

  delay(20);  // Riduce il carico
}
