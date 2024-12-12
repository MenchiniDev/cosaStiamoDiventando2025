
#include <Wire.h>
#include <ArduinoNunchuk.h>

#define BAUDRATE 9600

ArduinoNunchuk nunchuk = ArduinoNunchuk();

void setup()
{
  Serial.begin(BAUDRATE);
  Wire.setClock(100000);  // Imposta la velocità a 400 kHz (opzionale)
  nunchuk.init();
}

void loop()
{
  nunchuk.update();
  
  Serial.print("ASSE X ");
  Serial.print(nunchuk.analogX, DEC);
  Serial.print(" ASSE Y");
  Serial.print(' ');
  Serial.print(nunchuk.analogY, DEC);
  Serial.print(" ACCELL X");
  Serial.print(' ');
  Serial.print(nunchuk.accelX, DEC);
  Serial.print(" ACCELL Y");
  Serial.print(' ');
  Serial.print(nunchuk.accelY, DEC);
  Serial.print(" ACCELL Z");
  Serial.print(' ');
  Serial.print(nunchuk.accelZ, DEC);
  Serial.print(" BOTTONE Z PREMUTO");
  Serial.print(' ');
  Serial.print(nunchuk.zButton, DEC);
  Serial.print(" BOTTONE C PREMUTO");
  Serial.print(' ');
  Serial.println(nunchuk.cButton, DEC);
  delay(200);
}
