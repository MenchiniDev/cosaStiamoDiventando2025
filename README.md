# Cosa Stiamo Diventando, Viareggio's 2025 Carnival

![Cosa Stiamo Diventando](./img/cosaStiamoDiventando.jpg)


# CosaStiamoDiventando

## Descrizione del Progetto
CosaStiamoDiventando è un sistema avanzato di controllo per la maschera singola "Cosa Stiamo Diventando" di Andrea Giulio Ciaramitaro, classificatasi al secondo posto al Carnevale di Viareggio 2025. Il progetto prevede l'utilizzo di tre microcontrollori che operano in sinergia per gestire i servomotori responsabili dei movimenti della maschera. Dopo numerose fasi di test e debug, è stato ottimizzato per garantire movimenti fluidi e reattivi.

## Struttura del Sistema
Il sistema si basa su tre microcontrollori, ognuno con un compito specifico:

1. **ESP1 - Lettura del Nunchuk e trasmissione seriale**
   - Stabilisce la comunicazione I2C con il Nunchuk.
   - Legge i valori dell'asse X e dei pulsanti C e Z.
   - Converte i dati in formato seriale e li invia agli altri microcontrollori.

   **Snippet di codice ESP1:**
   ```cpp
   #include <Wire.h>
   #include <ArduinoNunchuk.h>

   ArduinoNunchuk nunchuk;

   void setup() {
       Serial.begin(115200);
       Wire.begin();
       nunchuk.init();
   }

   void loop() {
       nunchuk.update();
       Serial.print(nunchuk.analogX);
       Serial.print(",");
       Serial.print(nunchuk.buttonC);
       Serial.print(",");
       Serial.println(nunchuk.buttonZ);
       delay(50);
   }
   ```

2. **Mega - Decodifica dei dati e controllo dell'asse X**
   - Riceve i dati seriali da ESP1.
   - Interpreta i valori e determina il movimento dei servomotori.
   - Implementa un ciclo sinusoidale per garantire movimenti fluidi.

   **Snippet di codice Mega:**
   ```cpp
   #include <Servo.h>

   Servo servo;
   int pos;

   void setup() {
       Serial.begin(115200);
       servo.attach(9);
   }

   void loop() {
       if (Serial.available()) {
           String data = Serial.readStringUntil('\n');
           pos = data.toInt();
           servo.write(map(pos, 0, 255, 0, 180));
       }
   }
   ```

3. **ESP2 - Controllo finale dei servomotori**
   - Riceve i comandi da Mega e applica le correzioni necessarie.
   - Controlla i servomotori e implementa un sistema di sicurezza per evitare malfunzionamenti.

   **Snippet di codice ESP2:**
   ```cpp
   #include <Servo.h>

   Servo servo;
   int pos;

   void setup() {
       Serial.begin(115200);
       servo.attach(5);
   }

   void loop() {
       if (Serial.available()) {
           String data = Serial.readStringUntil('\n');
           pos = data.toInt();
           if (pos >= 0 && pos <= 180) {
               servo.write(pos);
           }
       }
   }
   ```

## Funzionamento
1. **Lettura del Nunchuk**: ESP1 comunica con il Nunchuk, interpreta i comandi e li invia via seriale.
2. **Elaborazione dei dati**: Mega legge i dati e decide i movimenti da eseguire.
3. **Movimento dei servomotori**: ESP2 riceve i comandi e li applica ai servomotori.
4. **Sicurezza e Controllo**: Sono implementati controlli di errore e watchdog per garantire stabilità.

## Debug e Ottimizzazioni
- Test approfonditi sulla comunicazione seriale tra microcontrollori.
- Implementazione di filtri per ridurre i disturbi nei movimenti.
- Ottimizzazione delle sequenze di movimento per un controllo preciso e realistico.

## Dipendenze
- **ArduinoNunchuk** per la gestione del Nunchuk.
- **Wire** per la comunicazione I2C.
- **Servo** per il controllo dei servomotori.
- **Watchdog Timer** per la gestione della stabilità.

## Installazione e Utilizzo
1. Caricare il codice su ciascun microcontrollore.
2. Collegare il Nunchuk all'ESP1 e i servomotori agli altri microcontrollori.
3. Avviare il sistema e monitorare la comunicazione seriale.

## Contatti
Per supporto o domande, contattare il responsabile del progetto.

---

CosaStiamoDiventando continua a evolversi, con futuri miglioramenti per ottimizzare la stabilità e le prestazioni del sistema.

