/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com/esp8266-nodemcu-access-point-ap-web-server/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid     = "ESP8266-Access-Point";
const char* password = "password";

float current = 0.0;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const int CURRENT_PIN = A0;
const int ADC_MAX = 1023;
const long INTERVAL_MS = 100;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
  </style>
</head>
<body>
  <h2>ESP8266 Server</h2>
  <h2>Current</h2>
  <p id="current">%CURRENT%</p>
</body>
<script>
  setInterval(function ( ) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        document.getElementById("current").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "/current", true);
    xhttp.send();
  }, %INTERVAL_MS% ) ;
</script>
</html>)rawliteral";

String processor(const String& var){
  if (var == "CURRENT"){
    return String(current);
  } else if (var == "INTERVAL_MS") {
    return String(INTERVAL_MS);
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  
  Serial.print("Setting AP (Access Point)...");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  // send current value on request
  server.on("/current", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(current).c_str());
  });

  // Start server
  server.begin();
}
 
void loop(){
  static unsigned long prev_ms = 0;
  unsigned long curr_ms = millis();
  if (curr_ms - prev_ms >= INTERVAL_MS) {
    int current_read = analogRead(CURRENT_PIN);
    current = (float)current_read / ADC_MAX;
    prev_ms = curr_ms;
  }
}