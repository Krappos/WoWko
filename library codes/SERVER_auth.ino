
#include <WiFi.h>
#include <WebServer.h>

WebServer server(80);

const char* www_login = "<username>";
const char* www_pass  = "<heslo>";

void handleRoot() {
  if (!server.authenticate(www_login, www_pass)) {
    return server.requestAuthentication();
  }

  server.send(200, "text/plain", " ");
}

void setup() {
  Serial.begin(115200);
  
  WiFi.begin("Wokwi-GUEST", "");
  while (WiFi.status() != WL_CONNECTED) delay(100);
  
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
}

//http://login:heslo@ip