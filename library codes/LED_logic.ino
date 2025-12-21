//logika pre status led diod 

//led konštanty pre piny led diod
#define LED_OK 15
#define LED_WARNING 16
#define LED_ERROR 17

// Globálne premenné
unsigned long previousMillis = 0;
unsigned long ledInterval = 500; 
bool ledState = false;
bool blikanie = true;        
int aktualnaLedka = LED_ERROR; 

String consoleInput = "";

void setup() {
  Serial.begin(115200);
  pinMode(LED_OK, OUTPUT);
  pinMode(LED_WARNING, OUTPUT);
  pinMode(LED_ERROR, OUTPUT);

  mainAnimation();
  
  stavKritickaChyba(); 
}

void loop() {
  readSerialCommand(); // Skontroluj, či prišiel príkaz
  updateLedTimer();    // Ak treba blikať, blikaj
}
// ---------------------------------------------


// --- FUNKCIA 1: Čítanie príkazov ---
void readSerialCommand() {
  if (Serial.available() > 0) {
    consoleInput = Serial.readString();
    consoleInput.trim();
    
    if (consoleInput == "1") {
      stavServerBezi();
    } 
    else if (consoleInput == "2") {
      stavServerStartuje();
    } 
    else if (consoleInput == "3") {
      stavKritickaChyba();
    }
  }
}

// --- FUNKCIA 2: Obsluha časovača (Blikanie) ---
void updateLedTimer() {
  // Ak je blikanie vypnuté, funkcia sa okamžite ukončí a nezdržuje procesor
  if (!blikanie) return; 

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >=ledInterval) {
    previousMillis = currentMillis;

    ledState = !ledState;
    digitalWrite(aktualnaLedka, ledState);
  }
}

// Režim 1: Zelená svieti trvalo
void stavServerBezi() {
  blikanie = false;             
  vypniVsetkyLed();             // Reset LEDiek
  digitalWrite(LED_OK, HIGH);   // Zapneme len zelenú
  Serial.println("Stav: Server beží (OK)");
}

// Režim 2: Oranžová bliká (5s)
void stavServerStartuje() {
  vypniVsetkyLed();             // Reset LEDiek
  aktualnaLedka = LED_WARNING;  // Nastavíme cieľovú LED
  ledInterval = 5000;              // Nastavíme čas
  blikanie = true;              // Povolíme časovač
  ledState = LOW;               // Reset stavu blikania
  previousMillis = millis();    // Reset odpočítavania
  Serial.println("Stav: Server štartuje (Loading)");
}

// Režim 3: Červená bliká (0.5s)
void stavKritickaChyba() {
  vypniVsetkyLed();
  aktualnaLedka = LED_ERROR;
  ledInterval = 500;
  blikanie = true;
  ledState = LOW;
  previousMillis = millis();
  Serial.println("Stav: KRITICKÁ CHYBA!");
}

// Pomocná funkcia na zhasnutie všetkého
void vypniVsetkyLed() {
  digitalWrite(LED_OK, LOW);
  digitalWrite(LED_WARNING, LOW);
  digitalWrite(LED_ERROR, LOW);
}

// Tvoja pôvodná animácia
void mainAnimation(){
  delay(1000);
  digitalWrite(LED_OK, HIGH);
  delay(500);
  digitalWrite(LED_WARNING, HIGH);
  delay(500);
  digitalWrite(LED_ERROR, HIGH);
  delay(2000);
  digitalWrite(LED_OK, LOW);
  delay(500);
  digitalWrite(LED_WARNING, LOW);
  delay(500);
  digitalWrite(LED_ERROR, LOW);
  delay(500);
  digitalWrite(LED_OK, HIGH);
  digitalWrite(LED_WARNING, HIGH);
  digitalWrite(LED_ERROR, HIGH);
  delay(500);
  digitalWrite(LED_OK, LOW);
  digitalWrite(LED_WARNING, LOW);
  digitalWrite(LED_ERROR, LOW);
}

//pridanie funkcie pre konkrétnu ledku

//ak kod prejde cely loop a netrigérne sa žiadne zariadenie tak 
//vtedy sa automaticky spustí červená ledka a začne blikať

//inicializácia lediek na začiatku sa rozsvietia po jednej všetky 
//a postupne sa vypnú ✅

//zelená
//svietenie
//server beží

//oranžová

//kazdých 5 sekund blikne
//server sa zapína



//kritikal
//kazdých 0.5sekundy blikne
//nejde pripojiť n net
//stránka sa nedá spustiť
//zariadenie nejde pingnúť