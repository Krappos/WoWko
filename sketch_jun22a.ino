//libraries
#include <WiFi.h>       //<- wifi library
#include <WiFiUdp.h>    //<- wifi library (Magic Packet)
#include <WebServer.h>  // <- webServer
#include <ESP32Ping.h>  //<- device ping library

//files
#include "WebPage.h"

//pointer pre optimalizíciu kodu
// SSID Init + declaration
const char* ssid = "WebServerNet";
const char* password = "91575302";

// ping status
bool online = false;

// device MAC adress
uint8_t ServerMac[] = { 0x40, 0xA8, 0xF0, 0x2E, 0xB1, 0x8C };
// O4 -> testing - uint8_t ServerMac[] = { };

// timing
unsigned long startTime = millis();

const unsigned long interval = 10000;
unsigned long previousMillis = 0;

WebServer server(8080);            //opened port
IPAddress target(192, 168, 1, 6);  // IP ping server
WiFiUDP udp;

void Pinger() {
  (Ping.ping(target, 3)) ? online = true : online = false;
}

void handleRoot() {
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

  WiFi.begin(ssid, password);
  // Serial.print("Pripájam sa na Wi-Fi ");
  // O1 -> testing  - Serial.print(ssid);

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    //Serial.println("Wi-Fi sa nepripojilo.");
  }
  // O2 -> testing - Serial.println(WiFi.localIP());
}

void PingRepeat() {
  unsigned long currentMillis = millis();


  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    Pinger();
    // O3 -> testing - Serial.println("pingujem");
  }
}

void handleKlik() {
  sendWOL();
  server.send(200, "text/plain", "OK");
}
void setup() {

  Serial.begin(115200);
  delay(100);  //oneskorenie pre stabilitu seriálky

  WiFiConnect();

  server.on("/", handleRoot);
  server.on("/klik", handleKlik);
  server.begin();

  server.on("/online", HTTP_GET, []() {
    String json = String("{\"online\":") + (online ? "true" : "false") + "}";
    server.send(200, "application/json", json);
  });

  previousMillis = millis();
}

void loop() {

  server.handleClient();
  PingRepeat();
}

// testing logs
// O1 -> vypis ssid overenie spravnosti siete
// O2 -> vypis ip adresa siete
// O3 -> testovanie millis
// O4 -> testing mac free
