#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "secrets.h"

const int ledPin = 2;
WebServer server(80);

String page()
{
  return "<!DOCTYPE html>"
  "<html>"
  "<head>"
  "<style>"
  "body{background:#f5f5f5;font-family:Arial;text-align:center;}"
  ".container{display:flex;justify-content:center;gap:30px;margin-top:40px;}"
  ".card{background:white;width:300px;padding:20px;border-radius:10px;box-shadow:0 0 10px gray;}"
  "button,input{padding:10px;margin-top:10px;}"
  "input{width:200px;}"
  "</style>"
  "</head>"
  "<body>"
  "<h1>ESP Information</h1>"
  "<div class='container'>"
  "<div class='card'>"
  "<h2>WiFi Networks</h2>"
  "<form action='/scan'>"
  "<button>Scan Networks</button>"
  "</form>"
  "%NETWORKS%"
  "</div>"
  "<div class='card'>"
  "<h2>LED Control</h2>"
  "<form action='/send'>"
  "<input type='text' name='led' placeholder='Type on/off'><br>"
  "<button>Send</button>"
  "</form>"
  "<p>LED is <b>%STATUS%</b></p>"
  "</div>"
  "</div>"
  "</body>"
  "</html>";
}

void handleRoot()
{
  String html = page();
  html.replace("%NETWORKS%", "Click Scan Networks");
  
  if(digitalRead(ledPin))
    html.replace("%STATUS%","ON");
  else
    html.replace("%STATUS%","OFF");
  
  server.send(200,"text/html",html);
}

void handleScan()
{
  int n = WiFi.scanNetworks();
  String html = page();
  String list = "";
  
  for(int i=0; i<n; i++)
  {
    list += WiFi.SSID(i);
    list += " (";
    list += WiFi.RSSI(i);
    list += " dBm)<br>";
  }
  
  html.replace("%NETWORKS%", list);
  
  if(digitalRead(ledPin))
    html.replace("%STATUS%", "ON");
  else
    html.replace("%STATUS%", "OFF");
  
  server.send(200,"text/html",html);
}

void handleSend()
{
  String value = server.arg("led");
  
  if(value == "on")
    digitalWrite(ledPin, HIGH);
  if(value == "off")
    digitalWrite(ledPin, LOW);
  
  server.sendHeader("Location","/");
  server.send(303);
}

void handleOn(){
  digitalWrite(ledPin, HIGH);
  server.sendHeader("location", "/");
  server.send(303);
}

void handleOff(){
  digitalWrite(ledPin, LOW);
  server.sendHeader("location", "/");
  server.send(303);
}

void setup()
{
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());
  
  server.on("/", handleRoot);
  server.on("/scan", handleScan);
  server.on("/send", handleSend);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  
  server.begin();
}

void loop()
{
  server.handleClient();
}
