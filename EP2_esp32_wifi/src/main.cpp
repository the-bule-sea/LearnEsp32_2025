#include <Arduino.h>
#include <WiFi.h>


// put function declarations here:
void WiFi_Connect()
{
  WiFi.begin("Mi10Pro", "88888888");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.println("Connecting..");

  WiFi_Connect();

if (WiFi.status() == WL_CONNECTED) {
    Serial1.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi connection failed");
  }
}

void loop() {
static unsigned long lastTime = 0;
  if (millis() - lastTime > 5000) {  // 每 5 秒输出一次
    lastTime = millis();
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
}
