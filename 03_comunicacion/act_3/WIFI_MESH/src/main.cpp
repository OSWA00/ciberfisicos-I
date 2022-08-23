#include "painlessMesh.h"

#define MESH_PREFIX "equipouno"
#define MESH_PASSWORD "password"
#define MESH_PORT 5555

Scheduler userScheduler; // Scheduler
painlessMesh mesh;

// Broadcast
void sendMessage();

Task taskSendMessage(TASK_SECOND * 5, TASK_FOREVER, &sendMessage);

void sendMessage()
{
#if ARDUINOJSON_VERSION_MAJOR == 6
  DynamicJsonDocument jsonBuffer(1024);
  JsonObject msg = jsonBuffer.to<JsonObject>();
#else
  DynamicJsonBuffer jsonBuffer;
  JsonObject &msg = jsonBuffer.createObject();
#endif
  msg["header"] = "Message_from_server";
  msg["From_id"] = mesh.getNodeId();
  msg["body"] = "Hello World";

  String str;
#if ARDUINOJSON_VERSION_MAJOR == 6
  serializeJson(msg, str);
#else
  msg.printTo(str);
#endif
  mesh.sendBroadcast(str);

#if ARDUINOJSON_VERSION_MAJOR == 6
  serializeJson(msg, Serial);
#else
  msg.printTo(Serial);
#endif
  Serial.printf("\n");
}

// Needed for painless library
void receivedCallback(uint32_t from, String &msg)
{
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId)
{
  Serial.printf("New Connection, nodeId = %u\n", nodeId);
}

void droppedConnectionCallback(uint32_t nodeId)
{
  Serial.printf("Lost Connection, nodeId = %u\n", nodeId);
}

void setup()
{
  Serial.begin(115200);

  // Configure mesh network
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 6);
  Serial.println("Mesh configured successful");

  // Set up callback
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onDroppedConnection(&droppedConnectionCallback);

  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();
}

void loop()
{
  // it will run the user scheduler as well
  mesh.update();
}
