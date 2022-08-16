#include <WiFi.h>
#include <esp_now.h>

#define LED 2
#define POT 36

uint8_t broadcastAddress[] = {0x78, 0xE3, 0x6D, 0x11, 0xE3, 0x8C};

// Reading to be sent
float pot_reading;

// If sending data was succesful
bool success;

typedef struct struct_message {
    float pot;
};

struct_message readings;
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
}

void loop() {
  // put your main code here, to run repeatedly:
  pot_reading = analogRead(POT);

  readings.pot = pot_reading / 4095 * 3.3;

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &readings, sizeof(readings));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(1000);
}
