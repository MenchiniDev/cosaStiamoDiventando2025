# Cosa Stiamo Diventando, Viareggio's 2025 Carnival

![Cosa Stiamo Diventando](./img/cosaStiamoDiventando.jpg)


# CosaStiamoDiventando

## Descrizione del Progetto
CosaStiamoDiventando è un sistema avanzato di controllo per la maschera singola "Cosa Stiamo Diventando" di Andrea Giulio Ciaramitaro, classificatasi al secondo posto al Carnevale di Viareggio 2025. Il progetto prevede l'utilizzo di tre microcontrollori che operano in sinergia per gestire i servomotori responsabili dei movimenti della maschera. Dopo numerose fasi di test e debug, è stato ottimizzato per garantire movimenti fluidi e reattivi.

## Struttura del Sistema
# Requisiti:
- 2 *ESP-12F*
- 1 *Arduino MEGA*
- 14 *MG 995* Servo motori
- 2 *PCA 8955* interfacce,
- 2 Batterie al piombo, 6v, 15Ah ciascuna
- Moduli step up e step down per alimentazione MEGA e ESP.
- cavi, saldatore, crimpatore.
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
   - Mappa i valori dell'asse x per i servo motori da lui controllati, attuatori del movimento degli occhi.
   - Gira le eventuali pressioni del tasto Z e C all' *ESP2*, il quale si occuperà della gestione.  
   - In caso di pressione del tasto C, implementa un ciclo sinusoidale per garantire movimenti continui.

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
   - Riceve i comandi da Mega e attua i servo motori responsabili dell'apertura e chiusura delle palpebre.
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
- Implementazione di filtri per ridurre i disturbi nei movimenti.
- Taratura per ogni servo motore del proprio gioco, modificando i valori in range del relativo movimento.
- Creazione di diversi sistemi di gestione delle criticità, tra cui:
- 1 -> in caso di malfunzionamento del Nunchuck, l' *ESP1* si occupa di inviare ai due controllori un segnale di errore,
-    impostando di conseguenza i movimenti sul ciclo, essendo impossibile raccogliere informazioni sull'asse X e palpebre da inviare ai servo motori.
- 2 -> in caso di malfunzionamento delle periferiche *PCA 8695*, come ad esempio una loro disconnessione, il *MEGA* e l'*ESP2* sono provvisti di una funzione che consente di ristabilire la connessione con la periferica. Se la connessione non viene stabilita entro 3 volte, o comunque entro 10 secondi, il micro controllore si riavvia, ricercando all'avvio la connessione con la periferica.
- 3 -> per ottenere un maggiore risparmio energetico e una maggiore conservazione dei servo motori, sono stati implementati diversi sistemi di preservazione, come la rimozione di segnali da inviare ai servo motori responsabili degli occhi, se il valore dell'asse X proveniente dal *Nunchuck* è nell'intorno del valore di equilibrio.
- 4 -> per garantire la sicurezza del circuito e un controllo umano al di sopra, è stato impostato un interruttore collegato all'alimentazione dei *PCA 8695* e dei controllori, garantendo un maggiore controllo del sistema di fronte a criticità.

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

