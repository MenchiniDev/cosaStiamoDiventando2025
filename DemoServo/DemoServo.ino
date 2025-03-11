#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Definisci il canale per il servo
const int servoChannels[7] = {0, 1, 2, 3, 4, 5, 6};

// Limiti del segnale PWM per i gradi del servo
const int SERVOMIN = 150;  // Pulse length per 0 gradi
const int SERVOMAX = 600;  // Pulse length per 180 gradi

void setup() {
  Serial.begin(9600);
  Serial.println("Avvio del sistema...");

  Wire.begin();  // Inizializza il bus I2C
  pwm.begin();   // Inizializza il driver PCA9685
  pwm.setPWMFreq(50);  // Frequenza per il controllo dei servo (in Hz)

  delay(100);
  Serial.println("Driver PCA9685 inizializzato.");
}

void loop() {
  // Porta il servo a 0 gradi
  Serial.println("Muovo i servo a 0 gradi...");
  for (int i = 0; i < 7; i++) {
    pwm.setPWM(servoChannels[i], 0, SERVOMIN);
    Serial.print("Servo ");
    Serial.print(i);
    Serial.println(" a 0 gradi.");
  }
  delay(1000);  // Aspetta 1 secondo

  // Porta il servo a 180 gradi
  Serial.println("Muovo i servo a 180 gradi...");
  for (int i = 0; i < 7; i++) {
    pwm.setPWM(servoChannels[i], 0, SERVOMAX);
    Serial.print("Servo ");
    Serial.print(i);
    Serial.println(" a 180 gradi.");
  }
  delay(1000);  // Aspetta 1 secondo

  Serial.println("Ciclo completato.");
}
