#include <Wire.h>
#include <ArduinoNunchuk.h>
#include <Adafruit_PWMServoDriver.h>  // Libreria per PCA9685

#define BAUDRATE 9600

ArduinoNunchuk nunchuk = ArduinoNunchuk();
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);  // PCA del Mega

const int servoChannels[7] = {0, 1, 2, 3, 4, 5, 6};
const int minPulse = 150;
const int maxPulse = 400;

bool cButtonState = false;
bool lastCButtonState = false;
bool zButtonState = false;
bool lastZButtonState = false;

void setup() {
  Serial.begin(BAUDRATE);    // Debug su monitor seriale
  Serial1.begin(BAUDRATE);   // Comunicazione UART verso il Nano
  Wire.setClock(100000);     // Imposta velocità I²C a 100 kHz
  pwm.begin();
  pwm.setPWMFreq(50);        // Frequenza per i servo
  nunchuk.init();
  Serial.println("\nMega pronto. Nunchuk inizializzato.");
}

void loop() {
  nunchuk.update();

  // Leggi i pulsanti
  cButtonState = nunchuk.cButton;
  zButtonState = nunchuk.zButton;

  // Invia lo stato dei pulsanti al Nano se cambia
  if (cButtonState != lastCButtonState) {
    Serial1.println(cButtonState ? "C:PRESSED" : "C:RELEASED");
    Serial.println(cButtonState ? "C:PRESSED" : "C:RELEASED");
    lastCButtonState = cButtonState;
  }

  if (zButtonState != lastZButtonState) {
    Serial1.println(zButtonState ? "Z:PRESSED" : "Z:RELEASED");
    Serial.println(zButtonState ? "Z:PRESSED" : "Z:RELEASED");
    lastZButtonState = zButtonState;
  }

  // Aggiorna i servo basandosi su analogX
  int joyX = nunchuk.analogX;
  int servoPulse = map(joyX, 0, 255, minPulse, maxPulse);
  for (int i = 0; i < 7; i++) {
    pwm.setPWM(servoChannels[i], 0, servoPulse);
  }

  delay(150);
}
