#include <WiFi.h>
#include "time.h"  

const char* ssid = "";
const char* password = "";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600; 
const int   daylightOffset_sec = 3600; 

void setup() {
  Serial.begin(115200); 

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("\nPripojené k WiFi!");

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
  struct tm timeinfo; 


if(getLocalTime(&timeinfo)){
    int hour = timeinfo.tm_hour;
    int min = timeinfo.tm_min;
    int sec = timeinfo.tm_sec;
}


if(!getLocalTime(&timeinfo)){
    Serial.println("Čakám na synchronizáciu s NTP...");

}else{
    Serial.println(&timeinfo, "%H:%M:%S"); 

}
  
  delay(1000);
}