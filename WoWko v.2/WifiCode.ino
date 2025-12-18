#include <WiFi.h>

const char* ssid = "Wokwi-GUEST";     
const char* password = "";  

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;      
const int   daylightOffset_sec = 3600; 

void printLocalTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Chyba: Nepodarilo sa získať čas.");
    return;
  }
  
  Serial.println(&timeinfo, "%H:%M:%S");
}

void setup() {
  Serial.begin(9600);

  Serial.println("Pripájam sa k %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  delay(2000);
}

void loop() {
  printLocalTime();
  delay(1000); 
}