#include <Wire.h>
#include <ArduinoNunchuk.h>
#include <Adafruit_PWMServoDriver.h>  // Libreria per PCA9685

#define BAUDRATE 9600

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CODICE IN SVILUPPO CON AGGIUNTA COSTANTE DI SERVO MOTORI
TODO: MIGLIORARE CHIUSURA PALPEBRA
++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

ArduinoNunchuk nunchuk = ArduinoNunchuk();
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();  // Oggetto PCA9685

const int servoXChannel = 0;     // Canale del PCA9685 per il servo asse X originale
const int servoYChannel = 1;     // Canale del PCA9685 per il servo asse Y originale
const int servoZChannel = 2;     // Canale del PCA9685 per il servo controllato dal pulsante Z

const int servoXMirrorChannel = 3;  // Canale del PCA9685 per il servo speculare dell'asse X
const int servoYMirrorChannel = 4;  // Canale del PCA9685 per il servo speculare dell'asse Y

const int minPulse = 0;        // Valore PWM minimo per il servo (angolo 0)
const int maxPulse = 500;        // Valore PWM massimo per il servo (angolo 60)

bool zButtonPressed = false;     // Flag per debounce del pulsante Z

void setup()
{
  Serial.begin(BAUDRATE);
  Wire.setClock(400000);  // Imposta velocità I²C a 400 kHz (opzionale)
  
  // Inizializzazione PCA9685
  pwm.begin();            
  pwm.setPWMFreq(50);     // Imposta la frequenza a 50 Hz per il controllo servo
  Serial.println("PCA9685 inizializzato con frequenza PWM a 50 Hz");

  // Inizializzazione Nunchuk
  nunchuk.init();
  Serial.println("Nunchuk inizializzato correttamente");

  // Stampa di debug per confermare l'avvio del sistema
  Serial.println("Setup completato");
}

void loop()
{
  // Aggiorna i dati dal Nunchuk e controlla se è stato letto correttamente
  nunchuk.update();
  //Serial.println("Dati Nunchuk aggiornati");

  // Lettura dell'asse X del joystick
  int joyX = nunchuk.analogX;
  Serial.print("Valore Asse X dal joystick: ");
  Serial.println(joyX);

  // Mappatura del valore di joyX in un segnale PWM per il servo asse X
  int servoXPulse = map(joyX, 255, 0, minPulse, maxPulse);
  pwm.setPWM(servoXChannel, 0, servoXPulse);
  

  // Controllo del pulsante Z con debounce
  if (nunchuk.zButton) {
    zButtonPressed = true;  // Setta il flag per evitare doppi trigger
    Serial.println("Pulsante Z rilevato come premuto.");

    // Muovi il servo Z a 180 gradi
    pwm.setPWM(servoZChannel, 0, 300);  

    // Riporta il servo Z a 0 gradi
    pwm.setPWM(servoZChannel, 0, 0);
    Serial.println("Servo su canale 2 riportato a 0 gradi");
    delay(500);

  } else if (!nunchuk.zButton && zButtonPressed) {
    // Resetta il flag se il pulsante Z viene rilasciato
    zButtonPressed = false;
  }
  
  // Debug di altri dati del Nunchuk
  //Serial.print("Stato Bottone C: ");
  //Serial.println(nunchuk.cButton);

  delay(100);

}
