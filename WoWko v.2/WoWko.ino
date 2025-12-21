//libraries
#include <WiFi.h>       //<- wifi library
#include <WiFiUdp.h>    //<- wifi library (Magic Packet)
#include <WebServer.h>  // <- webServer
#include <ESP32Ping.h>  //<- device ping library
#include "Time.h"
#include "secrets.h"

//files
#include "WebPage.h"

#define LED_OK 15
#define LED_WARNING 16 
#define LED_ERROR 17

// ping status
bool online = false;

// device MAC adress
uint8_t ServerMac[] = { xxxx, xxxx, xxxx, xxxx,xxxx, xxxx };
// O4 -> testing - uint8_t ServerMac[] = { };

// timing
unsigned long startTime = millis();

const unsigned long interval = 10000; //10 sekund
unsigned long ledInterval = 500; 

unsigned long previousMillisLed = 0;
unsigned long previousMillis = 0;


bool ledState = false;
bool blikanie = true;        
int aktualnaLedka = LED_ERROR; 
bool zapisaSa= false;

WebServer server(8080);            //opened port
IPAddress target(xxx, xxx, xxxx, xxx);  // IP ping server
WiFiUDP udp;

void handleRoot() {

   if (!server.authenticate(SECRET_NAME, SECRET_PASS)) {
    return server.requestAuthentication();
  }

  server.send(200, "text/html", htmlPage);

}


void sendWOL() {
  IPAddress bcastIP = ~WiFi.subnetMask() | WiFi.localIP();
  const int port = 9;
  byte packet[102];
  memset(packet, 0xFF, 6);
  for (int i = 0; i < 16; i++) {
    memcpy(&packet[6 + i * 6], ServerMac, 6);
  }

  udp.beginPacket(bcastIP, port);
  udp.write(packet, sizeof(packet));
  udp.endPacket();
  Serial.println("🌙 Wake-on-LAN paket odoslaný!");
}



void WiFiConnect() {
  unsigned long startAttemptTime = millis();

  WiFi.begin(SECRET_SSID, SECRES_WIFI_PASS);
  // O1 -> testing  - Serial.print(ssid);

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
  }
  // O2 -> testing - Serial.println(WiFi.localIP());
}




void PingRepeat() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if(Ping.ping(target, 3)){
online = true;
stavServerBezi();
zapisaSa=false;
    }
    else if(zapisaSa){
stavServerNebezi();
    }
    else{
online = false;

    }
    // O3 -> testing - Serial.println("pingujem");
  }
}


void handleKlik() {
  sendWOL();
  stavServerStartuje();
  server.send(200, "text/plain", "OK");
}

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
void stavServerBezi() {
  blikanie = false;             
  vypniVsetkyLed();             // Reset LEDiek
  digitalWrite(LED_OK, HIGH);   // Zapneme len zelenú
  Serial.println("Stav: Server beží (OK)");
}

// Režim 2: Oranžová bliká (5s)
void stavServerStartuje() {
  vypniVsetkyLed();             // Reset LEDiek
  aktualnaLedka = LED_OK;  // Nastavíme cieľovú LED
  ledInterval = 5000;              // Nastavíme čas
  blikanie = true;              // Povolíme časovač
  ledState = LOW;               // Reset stavu blikania
  previousMillis = millis();    // Reset odpočítavania
  Serial.println("Stav: Server štartuje (Loading)");
}
void stavServerNebezi() {
  vypniVsetkyLed();             // Reset LEDiek
  aktualnaLedka = LED_OK;  // Nastavíme cieľovú LED
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


void vypniVsetkyLed() {
  digitalWrite(LED_OK, LOW);
  digitalWrite(LED_WARNING, LOW);
  digitalWrite(LED_ERROR, LOW);
}

void updateLedTimer() {
  // Ak je blikanie vypnuté, funkcia sa okamžite ukončí a nezdržuje procesor
  if (!blikanie) return; 

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillisLed >=ledInterval) {
    previousMillis = currentMillis;

    ledState = !ledState;
    digitalWrite(aktualnaLedka, ledState);
  }
}

void setup() {

  Serial.begin(115200);
  delay(100);  //oneskorenie pre stabilitu seriálky

  
   pinMode(LED_OK, OUTPUT);
  pinMode(LED_WARNING, OUTPUT);
  pinMode(LED_ERROR, OUTPUT);

  WiFiConnect();

  server.on("/", handleRoot);
  server.on("/klik", handleKlik);
  server.on("/online", HTTP_GET, []() {
    String json = String("{\"online\":") + (online ? "true" : "false") + "}";
    server.send(200, "application/json", json);
  });

  server.begin();

  previousMillis = millis();

  mainAnimation();
  
  stavKritickaChyba(); 

}

void loop() {

  server.handleClient();
  PingRepeat();
  updateLedTimer();

}

// testing logs
// O1 -> vypis ssid overenie spravnosti siete
// O2 -> vypis ip adresa siete
// O3 -> testovanie millis
// O4 -> testing mac free

// úprava kodu a finalizácia
//úprava  metoda na preporčet času všetkých uživateľov nie local


//autentifikacia -> overenie cez http auth ✅
///led status -> pridanie lediek ku chybám 
//NTP status 