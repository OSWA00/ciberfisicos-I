// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"

//#include <Wire.h>


// Set your access point network credentials
const char* ssid = "ESP32-equipo1";
const char* password = "123456789";

/*#include <SPI.h>
#define BME_SCK 18
#define BME_MISO 19
#define BME_MOSI 23
#define BME_CS 5*/

float volt;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String readVolt() {
  volt=analogRead(36)/4095.0*3.3;
  return String(volt);
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.println();
  
  // Setting the ESP as an access point
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/voltaje", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readVolt().c_str());
  });
  
  bool status;

  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  
  // Start server
  server.begin();
}
 
void loop(){
  
}
