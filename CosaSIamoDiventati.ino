#include <Wire.h>
#include <ArduinoNunchuk.h>
#include <Servo.h>

// da modificare poi i servo X e Y e metterne 14 :)
Servo servo0X;  // Servo per movimento sull'asse X
Servo servo1X;  // Servo per movimento sull'asse X
Servo servo2X;  // Servo per movimento sull'asse X
Servo servo3X;  // Servo per movimento sull'asse X
Servo servo4X;  // Servo per movimento sull'asse X
Servo servo5X;  // Servo per movimento sull'asse X
Servo servo6X;  // Servo per movimento sull'asse X

Servo servo0Y; // Servo per il movimento sull'asse Y
Servo servo1Y; // Servo per il movimento sull'asse Y
Servo servo2Y; // Servo per il movimento sull'asse Y
Servo servo3Y; // Servo per il movimento sull'asse Y
Servo servo4Y; // Servo per il movimento sull'asse Y
Servo servo5Y; // Servo per il movimento sull'asse Y
Servo servo6Y; // Servo per il movimento sull'asse Y

// Aggiungi più servo per le palpebre
Servo servoP0;
Servo servoP1;
Servo servoP2;
Servo servoP3;
Servo servoP4;
Servo servoP5;
Servo servoP6;

int randomMov;  // Variabile di controllo per abilitare/disabilitare i movimenti random
ArduinoNunchuk nunchuk;

// Funzione per chiudere una palpebra a caso
void closeEye(int randomE, int randomT) {
  switch (randomE) {
    case 0:
      servoP0.write(180);
      delay(randomT);
      servoP0.write(0);
      break;
    case 1:
      servoP1.write(180);
      delay(randomT);
      servoP1.write(0);
      break;
    case 2:
      servoP2.write(180);
      delay(randomT);
      servoP2.write(0);
      break;
    case 3:
      servoP3.write(180);
      delay(randomT);
      servoP3.write(0);
      break;
    case 4:
      servoP4.write(180);
      delay(randomT);
      servoP4.write(0);
      break;
    case 5:
      servoP5.write(180);
      delay(randomT);
      servoP5.write(0);
      break;
    case 6:
      servoP6.write(180);
      delay(randomT);
      servoP6.write(0);
      break;
  }
}

void setup() {
  Serial.begin(9600);  // Inizializza la comunicazione seriale
  nunchuk.init();      // Inizializza il Nunchuk
  
  randomMov = 0;  // Inizialmente, i movimenti random sono disabilitati
	
  // Collega i servi ai pin
  servo0X.attach(2);  
  servo1X.attach(3);  
  servo2X.attach(4);  
  servo3X.attach(5);  
  servo4X.attach(6);  
  servo5X.attach(7);  
  servo6X.attach(8);  

  servo0Y.attach(9); 
  servo1Y.attach(10); 
  servo2Y.attach(11); 
  servo3Y.attach(12); 
  servo4Y.attach(13); 
  servo5Y.attach(14); 
  servo6Y.attach(15); 

  servoP0.attach(16);
  servoP1.attach(17);
  servoP2.attach(18);
  servoP3.attach(19);
  servoP4.attach(20);
  servoP5.attach(21);
  servoP6.attach(22);

  // azzero

  servo0X.write(0);  
  servo1X.write(0);  
  servo2X.write(0);  
  servo3X.write(0);  
  servo4X.write(0);  
  servo5X.write(0);  
  servo6X.write(0); 

  servo0Y.write(0); 
  servo1Y.write(0); 
  servo2Y.write(0); 
  servo3Y.write(0); 
  servo4Y.write(0); 
  servo5Y.write(0); 
  servo6Y.write(0);
  
  servoP0.write(0);
  servoP1.write(0);
  servoP2.write(0);
  servoP3.write(0);
  servoP4.write(0);
  servoP5.write(0);
  servoP6.write(0);
}

void loop() {
  nunchuk.update();  // Legge i dati dal Nunchuk

  // alloco i random sull'occhio da chiudere e per quanto tempo
  int randomE = random(0, 6);  // Sceglie quale servo delle palpebre chiudere
  int randomT = random(600, 1400);  // Durata del movimento casuale

  // Leggi la posizione del joystick e mappa i valori (da 0 a 180 gradi)
  int posX = map(nunchuk.analogX, 0, 255, 0, 180);
  int posY = map(nunchuk.analogY, 0, 255, 0, 180);

  // Muovi i servi in base alla posizione del joystick (se i movimenti random sono disabilitati)
  if (randomMov == 0) {
    servoX.write(posX);
    servoY.write(posY);

    // Stampa la posizione del joystick per il debug
    Serial.print("X: ");
    Serial.print(posX);
    Serial.print(" Y: ");
    Serial.println(posY);
  }

  // Se premo Z, chiudo una palpebra casuale per un tempo casuale
  if (nunchuk.zButton) {
    Serial.println("Pulsante Z premuto - Palpebre chiuse");
    closeEye(randomE, randomT);  // Chiamo la funzione per chiudere l'occhio
  }

  // Se premo C, si attivano/disattivano movimenti randomici
  if (nunchuk.cButton) {
    Serial.println("Pulsante C premuto - Toggle movimenti randomici");

    // Alterna l'attivazione dei movimenti random
    randomMov = !randomMov;
    Serial.println(randomMov ? "Movimenti randomici attivati" : "Movimenti randomici disattivati");

    delay(300);  // Debounce per il pulsante C
  }

  // Se i movimenti random sono attivi, esegui movimenti casuali
  while (randomMov == 1) {

    // Controllo pulsante C per interrompere i movimenti randomici
    if (nunchuk.cButton) {
      randomMov = 0;  // Esce dai movimenti randomici
      Serial.println("Movimenti randomici disattivati");
      delay(300);  // Debounce per evitare letture multiple accidentali
      break;  // Esce dal ciclo while
    }

    int randomX = random(0, 180);  // Genera un angolo casuale per l'asse X
    int randomY = random(0, 180);  // Genera un angolo casuale per l'asse Y
    int randomPalpebra = random(0, 2);  // 0 o 1 per la posizione delle palpebre

    randomE = random(0, 6);  // Sceglie quale servo delle palpebre chiudere
    randomT = random(600, 1400);  // Durata del movimento casuale

    // Muovi i servi X e Y a posizioni casuali
    Serial.print("Movimento random - X: ");
    Serial.print(randomX);
    Serial.print(" Y: ");
    Serial.println(randomY);
    
    servoX.write(randomX);
    servoY.write(randomY);

    // Se il pulsante Z è premuto, chiude la palpebra
    if (nunchuk.zButton) {
      Serial.println("Pulsante Z premuto - Palpebre chiuse");
      closeEye(randomE, randomT);  // Chiamo la funzione per chiudere l'occhio
    }

    // Pausa casuale tra un movimento e l'altro (tra 500ms e 1500ms)
    int randomDelay = random(500, 1500);
    for (int i = 0; i < randomDelay; i += 100) {  // Riduci i ritardi per un controllo più frequente
      delay(100);
      if (nunchuk.cButton) {
        randomMov = 0;
        Serial.println("Movimenti randomici disattivati");
        break;
      }
    }
  }

  delay(100);  // Attesa per evitare letture troppo rapide
}
