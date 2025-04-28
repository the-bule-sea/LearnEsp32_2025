#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>
#include <PubSubClient.h>

// DHT11传感器设置
#define DHTPIN 13
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// WiFi设置
const char *ssid = "A_110";
const char *password = "11111111";

// MQTT设置
const char *mqtt_broker = "120.55.92.23";
const char *topic = "sensor/1/dh11";
const int mqtt_port = 1883;
const char *mqtt_username = "esp32";
const char *mqtt_password = "esp32";

WiFiClient espClient;
PubSubClient client(espClient);

// 回调函数：接收MQTT消息
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

  // 初始化DHT传感器
  dht.begin();

  // 连接到WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // 配置MQTT
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  // 连接到MQTT代理
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

  // 发布一个初始消息并订阅
  client.publish(topic, "Hi EMQ X I'm ESP32 ^^");
  client.subscribe(topic);
}

void loop() {
  // 处理MQTT消息
  client.loop();

  // 读取DHT11传感器的温度和湿度
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // 如果读取失败，打印错误信息
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // 打印读取的温湿度值
  Serial.printf("湿度: %.1f%% 温度: %.1f°C\n", h, t);

  // 将温湿度数据以 JSON 格式发布到 MQTT 服务器
  String payload = "{\"temperature\": " + String(t, 1) + ", \"humidity\": " + String(h, 1) + "}";
  client.publish(topic, payload.c_str());

  // 每隔4秒读取一次数据
  delay(4000);
}
