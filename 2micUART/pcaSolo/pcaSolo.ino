#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Inizializza il PCA9685 all'indirizzo 0x41
Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(0x40);

// Limiti del segnale PWM per i gradi dei servo
const int SERVOMIN = 150; // Valore PWM per 0 gradi
const int SERVOMAX = 600; // Valore PWM per 180 gradi

// Funzione per convertire angoli in valori PWM
int angleToPWM(int angle) {
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}

unsigned long previousTimeEyes = 0;
unsigned long previousTimePalps = 0;

int currentAngleEyes = 30;
int currentAnglePalps = 0;

int eyesDirection = 1;  // 1: aumenta, -1: diminuisce
int palpsDirection = 1; // 1: aumenta, -1: diminuisce

// Impostazioni di velocità per il movimento
const int eyesSpeed = 50;  // Tempo per il movimento graduale degli occhi
const int palpsSpeed = 10; // Tempo per il movimento veloce delle palpebre

void setup() {
  bool found41 = false;
  bool found40 = false;
  Serial.begin(9600);
  //Wire.begin(4,5);
  // Inizializza il PCA9685
  pca.begin();
  pca.setPWMFreq(50); // Frequenza per il controllo dei servo

  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("Dispositivo trovato all'indirizzo 0x");
      Serial.println(address, HEX);
      if(address == 0x41){
        found41 = true;
      }
      if(address == 0x40){
        found40 = true;
      }
      //deviceFound = true;
    } else if (error == 4) {
      Serial.print("Errore all'indirizzo 0x");
      Serial.print(address, HEX);
      Serial.println(" (Errore non noto)");
    }
  }
  if(found40)
    Serial.println("\nPCA0x40 FUNZIONANTE");
  else
    Serial.println("PCA0x40 NON FUNZIONANTE");
  
  if(found41)
    Serial.println("PCA0x41 FUNZIONANTE");
  else
    Serial.println("PCA0x41 NON FUNZIONANTE");


  delay(500); // Aspetta che il driver si stabilizzi
}

void loop() {
  unsigned long currentMillis = millis();

  // Movimento occhi (graduale, canali 0->7)
  if (currentMillis - previousTimeEyes >= eyesSpeed) {
    previousTimeEyes = currentMillis;
    currentAngleEyes += eyesDirection;

    // Inverte la direzione quando raggiunge i limiti
    if (currentAngleEyes >= 60 || currentAngleEyes <= 30) {
      eyesDirection = -eyesDirection;
    }

    // Muove i servo per gli occhi (canali 0->7)
    for (int channel = 0; channel < 8; channel++) {
      pca.setPWM(channel, 0, angleToPWM(currentAngleEyes));
    }
  }

  // Movimento palpebre (veloce, canali 8->15)
  if (currentMillis - previousTimePalps >= palpsSpeed) {
    previousTimePalps = currentMillis;
    currentAnglePalps += palpsDirection;

    // Inverte la direzione quando raggiunge i limiti
    if (currentAnglePalps >= 60 || currentAnglePalps <= 0) {
      palpsDirection = -palpsDirection;
    }

    // Muove i servo per le palpebre (canali 8->15)
    for (int channel = 8; channel < 16; channel++) {
      pca.setPWM(channel, 0, angleToPWM(currentAnglePalps));
    }
  }
}
