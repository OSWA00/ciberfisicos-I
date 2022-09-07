#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define POT 36
#define LED 2

#define SSID "equipo_1"
#define PASSWORD "password"
#define MQTT_SERVER "192.168.4.1"

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

void init_wifi();
void callback(char *topic, byte *message, unsigned int length);
void init_MQTT();
void reconnect();

void setup()
{
  // put your setup code here, to run once:

  Serial.begin(115200);
  pinMode(POT, INPUT);
  pinMode(LED, OUTPUT);

  init_wifi();
  init_MQTT();
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2500)
  {
    lastMsg = now;

    float voltage = analogRead(POT) * 3.3 / 4094;
    char voltage_string[8];
    dtostrf(voltage, 1, 2, voltage_string);
    client.publish("/equipo_1/voltage", voltage_string);
  }
}

void init_wifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void init_MQTT()
{
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32_EQUIPO_1"))
    {
      Serial.println("connected");
      // Subscribe
      client.subscribe("/LED");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if (messageTemp == String(1))
  {
    digitalWrite(LED, HIGH);
  }
  else if (messageTemp == String(0))
  {
    digitalWrite(LED, LOW);
  }
}
