#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Crea un'istanza del driver PCA9685
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

const int openPulse = 280;  // Valore PWM per palpebra aperta
const int closePulse = 370; // Valore PWM per palpebra chiusa

//occhi
// valore si decrementa a sinistra e auomenta a destra
const int minPulse = 250;
const int maxPulse = 500;

void setup() {
  //Wire.begin(4, 5);
  Serial.begin(9600); // Inizializza la comunicazione seriale
  Serial.println("Inizializzazione PCA9685...");
  
  pwm.begin();
  pwm.setPWMFreq(50); // Frequenza tipica per servomotori: 50 Hz

  delay(1000); // Attesa per garantire l'inizializzazione
}

void loop() {
  for (int channel = 0; channel < 16; channel++) {
    if(channel==3)
      pwm.setPWM(channel,0,280)
    else
      pwm.setPWM(channel, 0, openPulse)
    delay(100)
  }
  delay(500);
    for (int channel = 0; channel < 16; channel++) {
    if(channel==3)
      pwm.setPWM(channel,0,280)
    else
      pwm.setPWM(channel, 0, closePulse);
    delay(100)
  }
}
