
#include <WiFi.h>
#include <esp_now.h>
#include "ESPAsyncWebServer.h"
// Structure to keep the temperature and humidity data
// Is also required in the client to be able to save the data directly

const char* ssid = "ESP32-equipo1";
const char* password = "123456789";

float volt;
AsyncWebServer server(80);

typedef struct struct_message {
  float pot;
};
// Create a struct_message called myData
struct_message potData;
// callback function executed when data is received
void OnRecv(const uint8_t * mac,const uint8_t * incomingData, int len) {
  memcpy(&potData, incomingData, sizeof(potData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Pot: ");
  Serial.println(potData.pot);
  volt=potData.pot;
}
  String readVolt() {
  return String(volt);
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Setting the ESP as an access point
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.begin(ssid, password);

  IPAddress IP = WiFi.localIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/voltaje", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readVolt().c_str());
  });
  
  bool status;

  // default settings
  // (you can also pass in a Wire library object like &Wire2)

   // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_AP_STA);
  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("There was an error initializing ESP-NOW");
    return;
  
  // Start server
  server.begin();
  }
  
  // Once the ESP-Now protocol is initialized, we will register the callback function
  // to be able to react when a package arrives in near to real time without pooling every loop.
  esp_now_register_recv_cb(OnRecv);
}
void loop() {
}
