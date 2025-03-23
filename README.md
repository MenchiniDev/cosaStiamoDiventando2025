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

Per garantire un funzionamento stabile ed efficiente del sistema, sono state implementate diverse ottimizzazioni e misure di gestione delle criticità.

### Filtri e Tarature  
- Applicazione di filtri per ridurre i disturbi nei movimenti.  
- Taratura individuale di ogni servo motore, regolando il range di movimento per compensare eventuali giochi meccanici.  

### Gestione delle Criticità  
1. **Errore del Nunchuck**  
   - In caso di malfunzionamento del Nunchuck, l'`ESP1` invia un segnale di errore ai controllori (`MEGA` ed `ESP2`).  
   - Di conseguenza, i movimenti vengono impostati su un ciclo predefinito, poiché non è possibile raccogliere dati dall'asse X e dalle palpebre per il controllo dei servo motori.  

2. **Disconnessione delle periferiche PCA9685**  
   - Il `MEGA` e l'`ESP2` dispongono di una funzione di ripristino della connessione con i moduli PCA9685.  
   - Se la connessione non viene ristabilita entro 3 tentativi o 10 secondi, il microcontrollore si riavvia automaticamente e tenta di riconnettersi all'avvio.  

3. **Ottimizzazione del consumo energetico e della durata dei servo motori**  
   - Implementazione di strategie per ridurre il consumo energetico e preservare i servo motori.  
   - Se il valore dell'asse X del Nunchuck è nell'intorno del valore di equilibrio, i segnali per i servo motori responsabili del movimento degli occhi vengono disattivati.  

4. **Sicurezza del sistema**  
   - Per garantire un controllo manuale in caso di emergenza, è stato aggiunto un interruttore collegato all'alimentazione dei moduli PCA9685 e dei microcontrollori.  
   - Questo permette di disattivare rapidamente il sistema in caso di criticità.  

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

