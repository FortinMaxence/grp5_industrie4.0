#include "DHT.h"
#include "PubSubClient.h" // Connect and publish to the MQTT broker
#include <Arduino_JSON.h>

// Code for the ESP8266
#include "ESP8266WiFi.h"  // Enables the ESP8266 to connect to the local network (via WiFi)
#define DHTPIN D1         // Pin connected to the DHT sensor

#define DHTTYPE DHT22  // DHT11 or DHT22
DHT dht(DHTPIN, DHTTYPE);

// WiFi
const char* ssid = "TheLabIOT";                 // Your personal network SSID
const char* wifi_password = "Yaay!ICanTalkNow"; // Your personal network password

// MQTT
const char* mqtt_server = "192.168.1.4";  // IP of the MQTT broker
const char* topic = "ESIEA/grp5";
const char* mqtt_username = "grp5"; // MQTT username
const char* mqtt_password = "grp5"; // MQTT password
const char* clientID = "client"; // MQTT client ID

WiFiClient wifiClient;
PubSubClient client(mqtt_server, 1883, wifiClient); 


// Custom function to connet to the MQTT broker via WiFi
void connect_MQTT(){
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password (Setup Earlier in the Instructable)
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}


void setup() {
  Serial.begin(115200);
  dht.begin();
}

void loop() {
  connect_MQTT();
  Serial.setTimeout(2000);
  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(" %");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C");

  JSONVar data;
  data["temperature"] = String(t);
  data["humidity"] = String(h);
  String jsonString = JSON.stringify(data);

  if (client.publish(topic, jsonString.c_str())) {
    Serial.println("Message sent!");
  }
  else {
    Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(topic, jsonString.c_str());
  }

  client.disconnect();  // disconnect from the MQTT broker
  delay(5000);       // print new values every 1 Minute
}