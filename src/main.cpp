
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "Ikkefaen";
const char* password = "Traktor100";

IPAddress serverip(10, 0, 0, 50);
ESP8266WebServer server(80);

const int led = 16;

void handleRoot() {
  digitalWrite(LED_BUILTIN, 0);
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp8266!");
  digitalWrite(LED_BUILTIN, 1);
}

void handleOn() {
  digitalWrite(LED_BUILTIN, 0);
  digitalWrite(led, 1);
  server.send(200, "text/plain", "esp8266 turning on!");
  digitalWrite(LED_BUILTIN, 1);
}

void handleOff() {
  digitalWrite(LED_BUILTIN, 0);
  digitalWrite(led, 0);
  server.send(200, "text/plain", "esp8266 turning off!");
  digitalWrite(LED_BUILTIN, 1);
}

void handleNotFound(){
  digitalWrite(LED_BUILTIN, 0);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(LED_BUILTIN, 1);
}

void setup(void){
  pinMode(led, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(led, 0);
  digitalWrite(LED_BUILTIN, 1);

  Serial.begin(115200);
  WiFi.begin(ssid, password);

  WiFi.config(serverip, IPAddress(193, 213, 112, 4), WiFi.gatewayIP(), WiFi.subnetMask());
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);

  server.on("/inline", [](){
    digitalWrite(LED_BUILTIN, 0);
    server.send(200, "text/plain", "this works as well");
    digitalWrite(LED_BUILTIN, 1);
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
