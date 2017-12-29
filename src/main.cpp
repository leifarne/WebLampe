
#include <Arduino.h>

#include <WifiManager.h>         //https://github.com/tzapu/WiFiManager
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "Ikkefaen";
const char* password = "Traktor100";
const char* wifiName = "SuperTrippelAP";
IPAddress serverip(10, 0, 0, 50);
ESP8266WebServer server(80);
WiFiManager wifiManager;

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
  Serial.println("esp8266 turning on!");
  digitalWrite(LED_BUILTIN, 1);
}

void handleOff() {
  digitalWrite(LED_BUILTIN, 0);
  digitalWrite(led, 0);
  server.send(200, "text/plain", "esp8266 turning off!");
  Serial.println("esp8266 turning off!");
  digitalWrite(LED_BUILTIN, 1);
}

void handleWifiReset() {
  server.send(200, "text/plain", "Manually starting configuration portal!");
  Serial.println("Manually starting configuration portal!");
  wifiManager.startConfigPortal(wifiName);

  Serial.println("Restarting ESP");
  ESP.restart();
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

  Serial.begin(9600);
  Serial.println("starting");

  wifiManager.resetSettings();  // For debugging, only.

  // Init Wifi - connect to existing, or configure.
  wifiManager.setSTAStaticIPConfig(serverip, IPAddress(10,0,0,138), IPAddress(255,255,255,0));
  wifiManager.setAPStaticIPConfig(serverip, serverip, IPAddress(255,255,255,0));

  //fetches ssid and pass from eeprom and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect(wifiName);

  // Configure server.
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
  server.on("/reset", handleWifiReset);

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
