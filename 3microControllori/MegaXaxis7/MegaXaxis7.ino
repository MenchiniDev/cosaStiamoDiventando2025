#include <Wire.h>
#include <ArduinoNunchuk.h>
#include <Adafruit_PWMServoDriver.h>  // Libreria per PCA9685

#define BAUDRATE 9600

ArduinoNunchuk nunchuk = ArduinoNunchuk();
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);  // Indirizzo PCA9685 (modifica se necessario)

const int servoChannels[7] = {0, 1, 2, 3, 4, 5, 6};  // Canali PCA per i servo X
const int minPulse = 150;  // Valore PWM minimo (corrisponde a 0°)
const int maxPulse = 600;  // Valore PWM massimo (corrisponde a 180°)

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
  int joyX = nunchuk.analogX;  // Leggi posizione joystick X
  Serial.println(nunchuk.analogX);
  Serial.println(joyX);
  int servoPulse = map(joyX, 0, 255, minPulse, maxPulse);

  for (int i = 0; i < 7; i++) {
    pwm.setPWM(servoChannels[i], 0, servoPulse);  // Muovi tutti i servo asse X
  }

  delay(200);  // Riduce il carico
}
