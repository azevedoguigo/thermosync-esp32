#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <WiFi.h>
#include <ArduinoWebsockets.h>

#include <ArduinoJson.h>

using namespace websockets;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

const float BETA = 3950;

const char* SSID = "Your WiFi network name";
const char* PASSWORD = "WiFi Password";
const char* WEBSOCKETS_SERVER_HOST = "192.168.0.108";
const uint16_t WEBSOCKETS_SERVER_PORT = 3000;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
WebsocketsClient client;

void setup() {
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("Fail to init display SSD1306"));
    for (;;);
  }

  display.clearDisplay();

  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("WiFi connecting...");
  }
  Serial.println("WiFi connected!");

  String ws_url = "ws://" + String(WEBSOCKETS_SERVER_HOST) + ":" + String(WEBSOCKETS_SERVER_PORT) + "/ws";

  bool connected = client.connect(ws_url);
  if (connected) {
      Serial.println("Connected to websocket server!");
  } else {
    Serial.println("Fail to connect websocket server... URL: " + ws_url);
  }
}

void loop() {
  int analogValue = analogRead(34);
  int celcius = 1 / (log(1 / (4095. / analogValue -1)) / BETA + 1.0 / 298.15) - 273.15;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(15, 1);
  display.println("Temperatura atual");

  display.setTextSize(2);
  display.setCursor(40, 35);
  display.print(celcius);

  display.setCursor(65, 30);
  display.write(248);

  display.setCursor(77, 35);
  display.println("C");

  display.display();
  

  if (client.available()) {
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["temperature"] = celcius;

    String jsonString;
    serializeJson(jsonDoc, jsonString);

    client.send(jsonString);
  }

  client.poll();
  delay(1000);
}
