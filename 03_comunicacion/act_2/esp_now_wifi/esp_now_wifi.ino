#include <esp_now.h>
#include <WiFi.h>
#include "ESPAsyncWebServer.h"

const char* ssid = "ESP32-equipo1";
const char* password = "123456789";

float incoming_voltage;

typedef struct message {
  float voltage;
};

message incoming_readings;

AsyncWebServer server(80);

void on_data_received(const uint8_t * mac_addr, const uint8_t * incoming_data, int len) {
  memcpy(&incoming_readings, incoming_data, sizeof(incoming_readings));
  incoming_voltage  = incoming_readings.voltage;
}

String read_voltage() {
 return String(incoming_voltage);  
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  WiFi.mode(WIFI_AP_STA);

  WiFi.begin(ssid, password);

  Serial.println(WiFi.softAPIP());
  
  Serial.println(WiFi.channel());
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(on_data_received);

  server.on("/voltage", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", read_voltage().c_str());
  });

  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:

}
