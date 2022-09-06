#include <Arduino.h>
// #include <WiFi.h>
// #include <PubSubClient.h>
#include <IMU.h>

#define DT 0.25
#define AA 0.97

#define A_GAIN 0.0573
#define G_GAIN 0.070

// Used by Kalman Filters
float Q_angle = 0.01;
float Q_gyro = 0.0003;
float R_angle = 0.01;
float x_bias = 0;
float y_bias = 0;
float XP_00 = 0, XP_01 = 0, XP_10 = 0, XP_11 = 0;
float YP_00 = 0, YP_01 = 0, YP_10 = 0, YP_11 = 0;
float KFangleX = 0.0;
float KFangleY = 0.0;

float kalmanFilterX(float accAngle, float gyroRate);
float kalmanFilterY(float accAngle, float gyroRate);

// #define SSID "equipo_1"
// #define PASSWORD "password"
// #define MQTT_SERVER "YOUR_MQTT_BROKER_IP_ADDRESS"

// WiFiClient espClient;
// PubSubClient client(espClient);

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

// void initWiFi();
// void callback(char *topic, byte *message, unsigned int length);
// void initMQTT();

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  // initWiFi();
  // initMQTT();

  enableIMU(); // Enable I2C IMU
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

  float kalmanX = kalmanFilterX(AccXangle, rate_gyr_x);
  float kalmanY = kalmanFilterY(AccYangle, rate_gyr_y);

  Serial.print("Kalman X: ");
  Serial.println(kalmanX);

  Serial.print("Kalman Y: ");
  Serial.println(kalmanY);

  // Complementary filter used to combine the accelerometer and gyro values.
  CFangleX = AA * (CFangleX + rate_gyr_x * DT) + (1 - AA) * AccXangle;
  CFangleY = AA * (CFangleY + rate_gyr_y * DT) + (1 - AA) * AccYangle;

  // Compute heading
  float heading = 180 * atan2(magRaw[1], magRaw[0]) / M_PI;

  // Convert heading to 0 - 360
  if (heading < 0)
    heading += 360;
  Serial.print("Heading: ");
  Serial.println(heading);

  // Normalize accelerometer raw values.
  float accXnorm = accRaw[0] / sqrt(accRaw[0] * accRaw[0] + accRaw[1] * accRaw[1] + accRaw[2] * accRaw[2]);
  float accYnorm = accRaw[1] / sqrt(accRaw[0] * accRaw[0] + accRaw[1] * accRaw[1] + accRaw[2] * accRaw[2]);

  // Calculate pitch and roll
  float pitch = asin(accXnorm);
  float roll = -asin(accYnorm / cos(pitch));

  float magXcomp = magRaw[0] * cos(pitch) - magRaw[2] * sin(pitch);
  float magYcomp = magRaw[0] * sin(roll) * sin(pitch) + magRaw[1] * cos(roll) + magRaw[2] * sin(roll) * cos(pitch);

  heading = 180 * atan2(magYcomp, magXcomp) / M_PI;

  // Convert heading to 0 - 360
  if (heading < 0)
    heading += 360;

  printf("Compensated  Heading %7.3f  \n", heading);

  // Serial.print("#AccX\t");
  // Serial.print(AccXangle);
  // Serial.print("\t###  AccY  ");
  // Serial.print(AccYangle);

  // Serial.print("  ###  GyrX\t");
  // Serial.print(gyroXangle);
  // Serial.print("  ###  GyrY  \t");
  // Serial.print(gyroYangle);
  // Serial.print("   ###  GyrZ\t");
  // Serial.print(gyroZangle);
  // Serial.print("     ######    CFangleX\t");
  // Serial.print(CFangleX);
  // Serial.print("   ######  CFangleY   \t");
  // Serial.print(CFangleY);
  // Serial.print("   ######  heading   \t");
  // Serial.print(heading);
  // Serial.print("    --Loop Time--\t");

  // Each loop should be at least 20ms.
  while (millis() - startTime < (DT * 1000))
  {
    delay(1);
  }
  // Serial.println(millis() - startTime);
}

// void initWiFi()
// {
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(SSID, PASSWORD);
//   Serial.print("Connecting to WiFi ..");
//   while (WiFi.status() != WL_CONNECTED)
//   {
//     Serial.print('.');
//     delay(1000);
//   }
//   Serial.println(WiFi.localIP());
// }

// void initMQTT()
// {
//   client.setServer(MQTT_SERVER, 1883);
//   client.setCallback(callback);
// }

// void callback(char *topic, byte *message, unsigned int length)
// {
//   Serial.print("Message arrived on topic: ");
//   Serial.print(topic);
//   Serial.print(". Message: ");
//   String messageTemp;

//   for (int i = 0; i < length; i++)
//   {
//     Serial.print((char)message[i]);
//     messageTemp += (char)message[i];
//   }
//   Serial.println();
// }

float kalmanFilterX(float accAngle, float gyroRate)
{
  float y, S;
  float K_0, K_1;

  KFangleX += DT * (gyroRate - x_bias);

  XP_00 += -DT * (XP_10 + XP_01) + Q_angle * DT;
  XP_01 += -DT * XP_11;
  XP_10 += -DT * XP_11;
  XP_11 += +Q_gyro * DT;

  y = accAngle - KFangleX;
  S = XP_00 + R_angle;
  K_0 = XP_00 / S;
  K_1 = XP_10 / S;

  KFangleX += K_0 * y;
  x_bias += K_1 * y;
  XP_00 -= K_0 * XP_00;
  XP_01 -= K_0 * XP_01;
  XP_10 -= K_1 * XP_00;
  XP_11 -= K_1 * XP_01;

  return KFangleX;
}

float kalmanFilterY(float accAngle, float gyroRate)
{
  float y, S;
  float K_0, K_1;

  KFangleY += DT * (gyroRate - y_bias);

  YP_00 += -DT * (YP_10 + YP_01) + Q_angle * DT;
  YP_01 += -DT * YP_11;
  YP_10 += -DT * YP_11;
  YP_11 += +Q_gyro * DT;

  y = accAngle - KFangleY;
  S = YP_00 + R_angle;
  K_0 = YP_00 / S;
  K_1 = YP_10 / S;

  KFangleY += K_0 * y;
  y_bias += K_1 * y;
  YP_00 -= K_0 * YP_00;
  YP_01 -= K_0 * YP_01;
  YP_10 -= K_1 * YP_00;
  YP_11 -= K_1 * YP_01;

  return KFangleY;
}
