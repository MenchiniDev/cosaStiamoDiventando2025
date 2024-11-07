#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Definisci il canale per il servo
const int servoChannel = 0;

// Limiti del segnale PWM per i gradi del servo
const int SERVOMIN = 150;  // Pulse length per 0 gradi
const int SERVOMAX = 600;  // Pulse length per 180 gradi

void setup() {
  Serial.begin(9600);
  pwm.begin();
  pwm.setPWMFreq(60);  // Frequenza per il controllo dei servo (in Hz)
}

void loop() {
  // Porta il servo a 0 gradi
  pwm.setPWM(servoChannel, 0, SERVOMIN);
  delay(1000);  // Aspetta 1 secondo
  
  // Porta il servo a 90 gradi
  int pulse90 = map(90, 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(servoChannel, 0, pulse90);
  delay(1000);  // Aspetta 1 secondo
  
  // Porta il servo a 180 gradi
  pwm.setPWM(servoChannel, 0, SERVOMAX);
  delay(1000);  // Aspetta 1 secondo
  
  // Torna a 0 gradi
  pwm.setPWM(servoChannel, 0, SERVOMIN);
  delay(1000);  // Aspetta 1 secondo
}
