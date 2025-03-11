#include <Wire.h>
#include <Adafruit_PWMServoDriver.h> // Libreria per PCA9685
#include <avr/wdt.h>                // Libreria per il Watchdog Timer

#define BAUDRATE 9600
#define LED_PIN 13

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40); // PCA del Mega

const int servoChannels[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9 ,10 ,11, 12, 13, 14, 15};
int minPulse[16] = {200, 150, 100, 150, 200, 100, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150};
int maxPulse[16] = {400, 400, 300, 400, 400, 350, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400};

bool sinusoidalActive = false;  // Stato del ciclo sinusoidale
unsigned long lastCycleTime = 0;
unsigned long cycleStartTime = 0;
bool cycleRunning = false;
int currentAngle = 250;
bool increasing = true;

void setup() {
  Serial3.begin(BAUDRATE);
  Serial1.begin(BAUDRATE);
  Serial.begin(BAUDRATE);
  Wire.setClock(400000);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  pwm.begin();
  pwm.setPWMFreq(50);

  wdt_enable(WDTO_8S);
  
  Serial.println("\nMega pronto.");
}

void loop() {
  wdt_reset();

  if (sinusoidalActive) {
    movementLeftRight();
    if (cycleRunning && millis() - cycleStartTime > 60000) {
      sinusoidalActive = false;
      cycleRunning = false;
      Serial.println("Ciclo terminato dopo 60 secondi");
    }
  }

  if (Serial3.available()) {
    String receivedData = Serial3.readStringUntil('\n');
    processESP1Data(receivedData);
  }

  delay(50);
}

void processESP1Data(const String &data) {
  int comma1 = data.indexOf(',');
  int comma2 = data.indexOf(',', comma1 + 1);
  String xValue = "";

  if (comma1 > 0 && comma2 > comma1) {
    xValue = data.substring(0, comma1);
    String cButtonState = data.substring(comma1 + 1, comma2);
    String zButtonState = data.substring(comma2 + 1);

    if (cButtonState == "1") {
      sinusoidalActive = true;
      cycleRunning = true;
      cycleStartTime = millis();
      Serial1.println("C:PRESSED");
    }
    if (zButtonState == "1") {
      sinusoidalActive = false;
      cycleRunning = false;
      Serial1.println("Z:PRESSED");
    }
  }

  int joyX = xValue.toInt();
  Serial.println(joyX);
  if (joyX > 50 && joyX < 150) {
    for (int i = 0; i < 16; i++) {
      pwm.setPWM(servoChannels[i], 0, 0);
    }
  } else {
    for (int i = 0; i < 16; i++) {
      int servoPulse = map(joyX, 0, 255, minPulse[i], maxPulse[i]);
      pwm.setPWM(servoChannels[i], 0, servoPulse);
    }
  }
}

void movementLeftRight() {
  int joyX = 40;
  for (int i = 0; i < 16; i++) {
    int servoPulse = map(joyX, 0, 255, minPulse[i], maxPulse[i]);
    pwm.setPWM(servoChannels[i], 0, servoPulse);
    //pwm.setPWM(servoChannels[i], 0, 0);
    delay(100);
  }
  delay(100);
  joyX = 180;
  for (int i = 0; i < 16; i++) {
    int servoPulse = map(joyX, 0, 255, minPulse[i], maxPulse[i]);
    pwm.setPWM(servoChannels[i], 0, servoPulse);
    //pwm.setPWM(servoChannels[i], 0, 0);
    delay(100);
  }
}
