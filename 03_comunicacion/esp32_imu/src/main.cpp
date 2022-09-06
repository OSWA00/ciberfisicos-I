#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <IMU.h>

#define DT 0.02
#define AA 0.97
#define G_GAIN 0.070

#define SSID "equipo_1"
#define PASSWORD "password"
#define MQTT_SERVER "YOUR_MQTT_BROKER_IP_ADDRESS"

WiFiClient espClient;
PubSubClient client(espClient);

byte buff[6];
int accRaw[3];
int magRaw[3];
int gyrRaw[3];
float rate_gyr_y = 0.0; // [deg/s]
float rate_gyr_x = 0.0; // [deg/s]
float rate_gyr_z = 0.0; // [deg/s]
float gyroXangle = 0.0;
float gyroYangle = 0.0;
float gyroZangle = 0.0;
float AccYangle = 0.0;
float AccXangle = 0.0;
float CFangleX = 0.0;
float CFangleY = 0.0;

unsigned long startTime;

void initWiFi();
void callback(char *topic, byte *message, unsigned int length);
void initMQTT();

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  initWiFi();
  initMQTT();
  delay(500);
  detectIMU();
  enableIMU();
}

void loop()
{
  // put your main code here, to run repeatedly:
  startTime = millis();

  // Read the measurements from  sensors
  readACC(buff);
  accRaw[0] = (int)(buff[0] | (buff[1] << 8));
  accRaw[1] = (int)(buff[2] | (buff[3] << 8));
  accRaw[2] = (int)(buff[4] | (buff[5] << 8));

  readMAG(buff);
  magRaw[0] = (int)(buff[0] | (buff[1] << 8));
  magRaw[1] = (int)(buff[2] | (buff[3] << 8));
  magRaw[2] = (int)(buff[4] | (buff[5] << 8));

  readGYR(buff);
  gyrRaw[0] = (int)(buff[0] | (buff[1] << 8));
  gyrRaw[1] = (int)(buff[2] | (buff[3] << 8));
  gyrRaw[2] = (int)(buff[4] | (buff[5] << 8));

  // Convert Gyro raw to degrees per second
  rate_gyr_x = (float)gyrRaw[0] * G_GAIN;
  rate_gyr_y = (float)gyrRaw[1] * G_GAIN;
  rate_gyr_z = (float)gyrRaw[2] * G_GAIN;

  // Calculate the angles from the gyro
  gyroXangle += rate_gyr_x * DT;
  gyroYangle += rate_gyr_y * DT;
  gyroZangle += rate_gyr_z * DT;

  // Convert Accelerometer values to degrees
  AccXangle = (float)(atan2(accRaw[1], accRaw[2]) + M_PI) * RAD_TO_DEG;
  AccYangle = (float)(atan2(accRaw[2], accRaw[0]) + M_PI) * RAD_TO_DEG;

  // If IMU is up the correct way, use these lines
  AccXangle -= (float)180.0;
  if (AccYangle > 90)
    AccYangle -= (float)270;
  else
    AccYangle += (float)90;

  // Complementary filter used to combine the accelerometer and gyro values.
  CFangleX = AA * (CFangleX + rate_gyr_x * DT) + (1 - AA) * AccXangle;
  CFangleY = AA * (CFangleY + rate_gyr_y * DT) + (1 - AA) * AccYangle;

  // Compute heading
  float heading = 180 * atan2(magRaw[1], magRaw[0]) / M_PI;

  // Convert heading to 0 - 360
  if (heading < 0)
    heading += 360;

  Serial.print("#AccX\t");
  Serial.print(AccXangle);
  Serial.print("\t###  AccY  ");
  Serial.print(AccYangle);

  Serial.print("  ###  GyrX\t");
  Serial.print(gyroXangle);
  Serial.print("  ###  GyrY  \t");
  Serial.print(gyroYangle);
  Serial.print("   ###  GyrZ\t");
  Serial.print(gyroZangle);
  Serial.print("     ######    CFangleX\t");
  Serial.print(CFangleX);
  Serial.print("   ######  CFangleY   \t");
  Serial.print(CFangleY);
  Serial.print("   ######  heading   \t");
  Serial.print(heading);
  Serial.print("    --Loop Time--\t");

  // Each loop should be at least 20ms.
  while (millis() - startTime < (DT * 1000))
  {
    delay(1);
  }
  Serial.println(millis() - startTime);
}

void initWiFi()
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

void initMQTT()
{
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
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
}
