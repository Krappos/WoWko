#include <WiFi.h>
#include <WiFiUdp.h>
#include <WebServer.h>
#include <ESP32Ping.h>
#include "Time.h"
#include "secrets.h"
#include "WebPage.h"

// --- KONFIGURÁCIA PINIOV ---
#define LED_OK 15      // ZELENÁ
#define LED_WARNING 16 // ORANŽOVÁ
#define LED_ERROR 17   // ČERVENÁ

// --- KONFIGURÁCIA MAC A IP ---
// ! NEZABUDNI TU DOPLNIŤ SVOJE ÚDAJE !
uint8_t ServerMac[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; 
IPAddress target(192, 168, 1, 10); 

// --- PREMENNÉ PRE STATUS ---
bool online = false;

// --- PREMENNE PRE NTP ---
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600; 
const int   daylightOffset_sec = 3600; 

time_t bootEndTime = 0;

// --- ČASOVAČE ---
const unsigned long pingInterval = 10000;         // Nastaviť interval pre ping (10s)
const unsigned long wifiReconnectInterval = 30000; // Nastaviť interval pre reconnect (30s)

unsigned long ledInterval = 500; 
unsigned long previousMillisLed = 0;
unsigned long previousMillisPing = 0; 
unsigned long previousMillisWifi = 0; 

// --- LED STAVY ---
bool ledState = false;
bool blikanie = true;        
int aktualnaLedka = LED_OK; 

// --- OBJEKTY ---
WebServer server(8080);
WiFiUDP udp;

// ==========================================
//           LED LOGIKA (FUNKCIE)
// ==========================================

// Vypnúť všetky LED diódy
void vypniVsetkyLed() {
  digitalWrite(LED_OK, LOW);
  digitalWrite(LED_WARNING, LOW);
  digitalWrite(LED_ERROR, LOW);
}

// Nastaviť stav: Server beží (Svietiť zeleno trvalo)
void stavServerBezi() {
  blikanie = false;             
  vypniVsetkyLed();             
  digitalWrite(LED_OK, HIGH);   
  Serial.println("LED: Zelená svieti (Online)");
}

// Nastaviť stav: Server nebeží (Blikať zeleno - Standby)
void stavServerNebezi() {
  if(aktualnaLedka == LED_OK && blikanie == true && ledInterval == 1000) return;

  vypniVsetkyLed();
  aktualnaLedka = LED_OK;       
  ledInterval = 1000;           // Nastaviť pomalé blikanie (1s)
  blikanie = true;              
  ledState = LOW;               
  previousMillisLed = millis();    
  Serial.println("LED: Zelená bliká (Offline/Standby)");
}

// Nastaviť stav: Server štartuje (Blikať oranžovo)
void stavServerStartuje() {
  vypniVsetkyLed();             
  aktualnaLedka = LED_WARNING;  
  ledInterval = 300;            // Nastaviť rýchlejšie blikanie (0.3s)
  blikanie = true;              
  ledState = LOW;               
  previousMillisLed = millis();    
  Serial.println("LED: Oranžová bliká (Štartujem)");
}

// Nastaviť stav: Chyba WiFi (Blikať červeno)
void stavWifiChyba() {
  vypniVsetkyLed();
  aktualnaLedka = LED_ERROR;
  ledInterval = 200;            // Nastaviť veľmi rýchle blikanie (Warning)
  blikanie = true;
  ledState = LOW;
  previousMillisLed = millis();
  Serial.println("LED: Červená bliká (Wi-Fi Error)");
}

// <--- OPRAVA: Pridaná chýbajúca funkcia, ktorú si volal v setupe
void stavKritickaChyba() {
  stavWifiChyba(); // Použijeme rovnakú logiku ako pri Wifi chybe
  Serial.println("LED: Kritická chyba (NTP/System)");
}

// ==========================================
//           HLAVNÁ LOGIKA
// ==========================================

void updateLedTimer() {
  if (!blikanie) return; 

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillisLed >= ledInterval) {
    previousMillisLed = currentMillis; 
    ledState = !ledState;
    digitalWrite(aktualnaLedka, ledState);
  }
}

void checkWifiConnection() {
  unsigned long currentMillis = millis();
  
  if (WiFi.status() != WL_CONNECTED) {
    if (currentMillis - previousMillisWifi >= wifiReconnectInterval) {
      Serial.println("⚠️ Wi-Fi vypadla. Pokúšam sa o Reconnect...");
      stavWifiChyba(); 
      
      WiFi.disconnect();
      WiFi.reconnect();
      
      previousMillisWifi = currentMillis;
    }
  }
}

void WiFiConnect() {
  WiFi.begin(SECRET_SSID, SECRTES_WIFI_PASS);
  Serial.print("Pripájam k WiFi");
  
  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 10000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Pripojené! IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Nepodarilo sa pripojiť. Pobeží reconnect v slučke.");
    stavWifiChyba();
  }
}

void sendWOL() {
  if(WiFi.status() != WL_CONNECTED) return; 

  IPAddress bcastIP = ~WiFi.subnetMask() | WiFi.localIP();
  byte packet[102];
  
  memset(packet, 0xFF, 6);
  for (int i = 0; i < 16; i++) {
    memcpy(&packet[6 + i * 6], ServerMac, 6);
  }
  
  udp.beginPacket(bcastIP, 9);
  udp.write(packet, sizeof(packet));
  udp.endPacket();
  Serial.println("🌙 WOL Paket odoslaný.");
}

void PingRepeat() {
  if(WiFi.status() != WL_CONNECTED) return;

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillisPing >= pingInterval) {
    previousMillisPing = currentMillis;

    Serial.print("Pingujem PC... ");
    if(Ping.ping(target, 3)){
      Serial.println("Je ONLINE!");
      online = true;
      stavServerBezi(); 
    }
    else{
      Serial.println("Je OFFLINE.");
      online = false;
      stavServerNebezi(); 
    }
  }
}

// ==========================================
//           WEB HANDLERS
// ==========================================

void handleRoot() {
   if (!server.authenticate(SECRET_NAME, SECRET_PASS)) {
    return server.requestAuthentication();
  }
  server.send(200, "text/html", htmlPage);
}

void handleKlik() {
  sendWOL();
  stavServerStartuje();
  previousMillisPing = millis(); 

  time_t now;
  time(&now);
  bootEndTime = now + 300; 

  server.send(200, "application/json", "{\"status\":\"OK\"}");
}

// ==========================================
//           SETUP & LOOP
// ==========================================

void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(LED_OK, OUTPUT);
  pinMode(LED_WARNING, OUTPUT);
  pinMode(LED_ERROR, OUTPUT);

  digitalWrite(LED_OK, HIGH); delay(300); digitalWrite(LED_OK, LOW);
  digitalWrite(LED_WARNING, HIGH); delay(300); digitalWrite(LED_WARNING, LOW);
  digitalWrite(LED_ERROR, HIGH); delay(300); digitalWrite(LED_ERROR, LOW);

  WiFiConnect();

  if (WiFi.status() == WL_CONNECTED) {
    configTime(3600, 3600, "pool.ntp.org");
    Serial.println("Čakám na čas...");

    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
       Serial.println("⚠️ Chyba: Nepodarilo sa získať čas z NTP!");
       stavKritickaChyba(); // <--- OPRAVA: Táto funkcia už existuje
    } else {
       Serial.println(&timeinfo, "Čas úspešne načítaný: %A, %B %d %Y %H:%M:%S");
    }
  }

  server.on("/", handleRoot);
  server.on("/klik", handleKlik);
  
  // <--- OPRAVA: Presunutá a vylepšená definícia /online DOVNÚTRA setup()
  server.on("/online", HTTP_GET, []() {
    time_t now;
    time(&now);
    
    long remaining = 0;
    
    if (!online && bootEndTime > now) {
      remaining = (long)(bootEndTime - now);
    } else {
      remaining = 0;
      if(online) bootEndTime = 0; 
    }

    String json = "{";
    json += "\"online\":" + String(online ? "true" : "false") + ",";
    json += "\"remaining\":" + String(remaining);
    json += "}";
    
    server.send(200, "application/json", json);
  });
  
  server.begin();
  
  if (WiFi.status() == WL_CONNECTED) {
    PingRepeat();
  }
}

void loop() {
  checkWifiConnection();

  if(WiFi.status() == WL_CONNECTED) {
     server.handleClient();
     PingRepeat();
  }

  updateLedTimer();
}