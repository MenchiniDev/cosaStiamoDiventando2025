#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>  // Libreria per PCA9685

#define BAUDRATE 9600
#define LED_PIN 2  // LED collegato al pin 2 dell'ESP

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);

const int servoChannels[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
const int openPulse = 250;  // PWM per palpebra aperta
const int closePulse = 400; // PWM per palpebra chiusa

bool cycleActive = false;
unsigned long cycleStartTime = 0;  // Tempo di inizio del ciclo
bool I2CErrorOccurred = false;
int i2cReconnectAttempts = 0;

void setup() {
    Wire.begin(4, 5);  // SDA = 4, SCL = 5
    Serial.begin(BAUDRATE);
    Wire.setClock(100000);  // Velocità I2C

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    pwm.begin();
    pwm.setPWMFreq(50);  // Frequenza per i servo

    ESP.wdtDisable();
    ESP.wdtEnable(1000);  // Watchdog con timeout di 1 secondo
}

void loop() {
    checkSerialCommands();
    if (cycleActive) {
        if (millis() - cycleStartTime > 60000) {  // Disattiva dopo 60 secondi
            cycleActive = false;
            resetServos();
            Serial.println("Ciclo terminato dopo 60 secondi");
        } else {
            controlLids();
        }
    }
    checkI2C();
    ESP.wdtFeed();  // Reset del watchdog
    delay(100);
}

void checkSerialCommands() {
    static String receivedData = "";

    while (Serial.available()) {
        char incomingChar = Serial.read();

        if (incomingChar == '\n') {
            receivedData.trim();
            parseSerialCommand(receivedData);
            receivedData = "";
        } else {
            receivedData += incomingChar;
        }
    }
}

void parseSerialCommand(const String &command) {
    if (command == "C:PRESSED") {
        Serial.println("C premuto");
        cycleActive = true;
        cycleStartTime = millis();  // Registra il tempo di inizio
    } else if (command == "Z:PRESSED") {
        Serial.println("Z premuto");
        cycleActive = false;
        closeAndOpenLids();
    }
}

void closeAndOpenLids() {
    for (int i = 0; i < 16; i++) {
        pwm.setPWM(servoChannels[i], 0, (i == 3) ? 280 : closePulse);
    }
    delay(200);
    for (int i = 0; i < 16; i++) {
        pwm.setPWM(servoChannels[i], 0, (i == 3) ? 370 : openPulse);
    }
}

void resetServos() {
    for (int i = 0; i < 16; i++) {
        pwm.setPWM(servoChannels[i], 0, (i == 3) ? 370 : openPulse);
    }
}

void checkI2C() {
    Wire.beginTransmission(0x41);
    if (Wire.endTransmission() != 0) {
        digitalWrite(LED_PIN, LOW);
        if (!I2CErrorOccurred) {
            I2CErrorOccurred = true;
        }
        if (++i2cReconnectAttempts > 5) ESP.restart();
    } else {
        I2CErrorOccurred = false;
        i2cReconnectAttempts = 0;
    }
}

void controlLids() {
    static unsigned long lastBlinkTime = 0;
    static int consecutiveBlinks = 0;
    static unsigned long nextBlinkDelay = 2000;
    unsigned long currentTime = millis();

    if (currentTime - lastBlinkTime >= nextBlinkDelay) {
        for (int i = 0; i < 2; i++) {
            closeAndOpenLids();
            delay(500);
        }

        for (int channel = 0; channel < 16; channel++) {
            pwm.setPWM(channel, 0, (channel == 3) ? 370 : openPulse);
            delay(100);
        }
        delay(500);

        for (int channel = 0; channel < 16; channel++) {
            pwm.setPWM(channel, 0, (channel == 3) ? 280 : closePulse);
            delay(100);
        }

        lastBlinkTime = currentTime;

        if (consecutiveBlinks < 2 && random(0, 3) > 1) {
            consecutiveBlinks++;
            nextBlinkDelay = random(300, 700);
        } else {
            consecutiveBlinks = 0;
            nextBlinkDelay = random(2000, 5000);
        }
    }
}
