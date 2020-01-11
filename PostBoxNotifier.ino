/*
  SimpleMQTTClient.ino
  The purpose of this exemple is to illustrate a simple handling of MQTT and Wifi connection.
  Once it connects successfully to a Wifi network and a MQTT broker, it subscribe to a topic and send a message to it.
  It will also send a message delayed 5 seconds later.
*/

#include "EspMQTTClient.h"
#include "Constants.h"

const int lightSensorAnalogData = A0; // Variable value can't be changed after initialization
const int lightSensorDigitalData = D1; // Variable value can't be changed after initialization

int sensorValue = 0;
String output;
String tmp;
char sendSensorData[100];


const char* ssid = GlobalConstants::ssid;
const char* pass = GlobalConstants::password;

EspMQTTClient client(
  GlobalConstants::ssid,
  GlobalConstants::password,
  GlobalConstants::ipAddress,  // MQTT Broker server ip
  "MQTTUsername",   // Can be omitted if not needed
  "MQTTPassword",   // Can be omitted if not needed
  "PostBox",     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);

void getStatus(const String & payload) {
  if (payload == "getStatus") {
    client.publish("Briefkasten/response", "Briefkasten is alive");
    
    //client.publish("Briefkasten/response", (unsigned char)sensorValue);
  }
}

void setup()
{
  Serial.begin(115200);

  // Optionnal functionnalities of EspMQTTClient :
  //client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  //client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overrited with enableHTTPWebUpdater("user", "password").
  client.enableLastWillMessage("TestClient/lastwill", "I am going offline");  // You can activate the retain flag by setting the third parameter to true
}

// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
void onConnectionEstablished()
{
  // Subscribe to "Briefkasten/test" and display received message to Serial
  client.subscribe("Briefkasten/test", [](const String & payload) {
    Serial.println(payload);
  });

  client.subscribe("Briefkasten/request", [](const String & payload) {
    getStatus(payload);
    Serial.println(payload);
  });
     
  
  // Publish a message to "Briefkasten/test"
  client.publish("Briefkasten/test", "Briefkasten is connected"); // You can activate the retain flag by setting the third parameter to true
  
  // Execute delayed instructions
  client.executeDelayed(5 * 1000, []() {
    client.publish("Briefkasten/test", "This is a message sent 5 seconds later");
  });

   //client.publish("Briefkasten/test", "loop",true); // You can activate the retain flag by setting the third parameter to true
}

void publishSensorData(){
  sensorValue = analogRead(lightSensorAnalogData);
  tmp = String(sensorValue);
  client.publish("Briefkasten/test", tmp); // You can activate the retain flag by setting the third parameter to true
  sensorValue = digitalRead(lightSensorDigitalData);
  tmp = String(sensorValue);
  client.publish("Briefkasten/test", tmp); // You can activate the retain flag by setting the third parameter to true
}

void loop()
{
  client.loop();
  delay(100);
  publishSensorData();
  
  
  
  //Serial.println(sensorValue);

  //sensorValue = digitalRead(lightSensorDigitalData);
  //Serial.println(sensorValue);
}
