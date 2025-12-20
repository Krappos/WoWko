#include <WiFi.h>
#include <WebServer.h>

WebServer server(80);

void handleRoot() {
  server.send(200, "text/plain", " <vypis na stranku> ");
}

void setup() {
  Serial.begin(115200);
  
  WiFi.begin("", ""); 
  while (WiFi.status() != WL_CONNECTED) delay(100);
  
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  
  server.begin();
}

void loop() {
    server.handleClient();
}

