#include <WiFi.h>

const char* ssid="Wokwi-GUEST";
const char* password="";

void setup() {

  Serial.begin(9600);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
 
}

void loop() {
  
}
