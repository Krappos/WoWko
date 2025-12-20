    #include <WiFi.h>

void setup() {

  Serial.begin(9600);
  WiFi.begin("<ssid>", "<password>");
  }
 
void loop() {}
