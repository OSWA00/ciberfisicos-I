#include <WiFi.h>
#include <esp_now.h>

#define LED 2
#define POT 36

uint8_t broadcastAddress[] = {0x30, 0xC6, 0xF7, 0x2F, 0x0A, 0x58};

// Reading to be sent
int pot_reading;
int incoming_pot;

// If sending data was succesful
bool success;

typedef struct struct_message {
    int pot;
};

struct_message readings;
struct_message incoming_readings;
esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
    if (status == 0) {
        success = true;
    }
    else {
        success = false;
    }
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incoming_readings, incomingData, sizeof(incoming_readings));
  Serial.print("Bytes received: ");
  Serial.println(len);
  incoming_pot = incoming_readings.pot;
  
  readings.pot = incoming_pot;

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &readings, sizeof(readings));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
}
 

void setup() {
    // Init Serial Monitor
    Serial.begin(115200);
    Serial.println("Hello");
    
    // Set peripherals
    pinMode(LED, OUTPUT);
    pinMode(POT, INPUT);
    
    // Set ESP32 as Wi-Fi station
    WiFi.mode(WIFI_STA);

    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
      Serial.println("Unable to init ESP-NOW");
      return;
    }
    Serial.println("Connected to ESP-NOW");
    digitalWrite(LED, HIGH);

    esp_now_register_send_cb(OnDataSent);
    
    // Register peer
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;
    
    // Add peer        
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
      Serial.println("Failed to add peer");
      return;
    }
    Serial.println("Peer added!");

    esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // put your main code here, to run repeatedly:
}
