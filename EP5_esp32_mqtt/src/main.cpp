#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// wifi
const char *ssid = "A_110";
const char *password = "11111111";

// mqttx
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "esp32";
const int mqtt_port = 1883;
const char *mqtt_username = "esp32";
const char *mqtt_password = "esp32@233";

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char *topic, byte *payload, unsigned int length) {
 Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 Serial.print("Message:");
 for (int i = 0; i < length; i++) {
     Serial.print((char) payload[i]);
 }
 Serial.println();
 Serial.println("-----------------------");
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
     String client_id = "esp32-client-";
     client_id += String(WiFi.macAddress());
     Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
     if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
         Serial.println("Public emqx mqtt broker connected");
     } else {
         Serial.print("failed with state ");
         Serial.print(client.state());
         delay(2000);
     }
 }
 // publish and subscribe
 client.publish(topic, "Hi EMQ X I'm ESP32 ^^");
 client.subscribe(topic);
}

void loop() {
  // put your main code here, to run repeatedly:
  client.loop();
}

// put function definitions here:
