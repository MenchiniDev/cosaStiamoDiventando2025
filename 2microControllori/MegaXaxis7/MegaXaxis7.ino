#include <Wire.h>
#include <ArduinoNunchuk.h>
#include <Adafruit_PWMServoDriver.h>  // Libreria per PCA9685

#define BAUDRATE 19200

ArduinoNunchuk nunchuk = ArduinoNunchuk();
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);  // Indirizzo PCA9685 (modifica se necessario)

const int servoChannels[7] = {0, 1, 2, 3, 4, 5, 6};  // Canali PCA per i servo X
const int minPulse = 150;  // Valore PWM minimo (corrisponde a 0°)
const int maxPulse = 500;  // Valore PWM massimo (corrisponde a 180°)

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

  //CICLO A PRESSIONE DI C
  if (cycleActive) {
    currentPulse = minPulse

    for (int i = 0; i < 7; i++) {
      pwm.setPWM(servoChannels[i], 0, minPulse);  // Muovi i servo a 0°
    }
    
    delay(100); 

    for (int i = 0; i < 7; i++) {
      currentPulse = minPulse + i*10;
      pwm.setPWM(servoChannels[i], 0, currentPulse); 
    }

    delay(100);

    for (int i = 0; i < 7; i++) {
      currentPulse = currentPulse - i*10;
      pwm.setPWM(servoChannels[i], 0, currentPulse);  // Muovi i servo a 180°
    }

    delay(100); 
  
    for (int i = 0; i < 7; i++) {
      pwm.setPWM(servoChannels[i], 0, maxPulse);  // Muovi i servo a 180°
    }

  } else {
    // Controllo normale del joystick
    int joyX = nunchuk.analogX;  // Leggi posizione joystick X
    int servoPulse = map(joyX, 0, 255, minPulse, maxPulse);
    for (int i = 0; i < 7; i++) {
      pwm.setPWM(servoChannels[i], 0, servoPulse);  // Muovi tutti i servo asse X
    }
  }

  delay(20);  // Riduce il carico
}
