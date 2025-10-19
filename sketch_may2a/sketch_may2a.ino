#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <PubSubClient.h>
#define SSD1306_WHITE 1

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 16
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pins
#define MAX_WAIT_FOR_TIMER 10
#define LED_PIN LED_BUILTIN
#define ADC_PIN 36
#define Bp_pin 23

// MQTT
WiFiClient espClient;
PubSubClient mqttClient(espClient);
const char* mqtt_server = "192.168.5.35";
const int mqtt_port = 1883;
const char* topic_photo = "photo";
const char* topic_led = "led";
String messageEcran = "";
unsigned long lastPublish = 0;

// WiFi
const char* ssid = "Saaad";
const char* password = "sbps2044";
bool wifiConnected = false;
bool wifiConnecting = false;
unsigned long connectStartTime = 0;
String wifiIP = "";

// Structs
struct mailbox_s {
  volatile int state;
  volatile int value;
  volatile int stop;
  volatile int cligo;
  volatile bool wifi_valid;
  volatile bool request_wifi;
};
struct oled_s {
  int timer;
  unsigned long period;
  int counter;
  mailbox_s *mb;
};
struct led_s {
  int timer;
  unsigned long period;
  int pin;
  mailbox_s *mb;
};
struct Photo_s {
  int timer;
  unsigned long period;
  int pin;
  mailbox_s *mb;
};

mailbox_s Mailbox1 = {0, 0, 0, 1000000, false, false};
struct oled_s oled1;
struct led_s led1;
struct Photo_s photo1;

// Timer
unsigned int waitFor(int timer, unsigned long period) {
  static unsigned long waitForTimer[MAX_WAIT_FOR_TIMER] = {0};
  unsigned long newTime = micros() / period;
  int delta = newTime - waitForTimer[timer];
  if (delta < 0) delta = 1 + newTime;
  if (delta) waitForTimer[timer] = newTime;
  return delta;
}

// MQTT callback
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  message.trim();
  Serial.printf("[MQTT] %s → %s\n", topic, message.c_str());

  if (message == "on") {
    Mailbox1.state = 1;
  } else if (message == "off") {
    Mailbox1.state = 0;
  }
  if (strcmp(topic, "ecran") == 0) {
    messageEcran = message;
  }
}

// Setup OLED
void setup_oled(struct oled_s *ctx, int timer, unsigned long period, mailbox_s *mb) {
  ctx->timer = timer;
  ctx->period = period;
  ctx->counter = 0;
  ctx->mb = mb;
  Wire.begin(4, 15);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    while (1);
  }
  display.clearDisplay();
  display.display();
}

// Setup LED
void setup_LED(struct led_s *ctx, int pin, mailbox_s *mb) {
  ctx->pin = pin;
  ctx->mb = mb;
  pinMode(pin, OUTPUT);
}

// Setup photoresistor
void setup_photo(struct Photo_s *ctx, int timer, unsigned long period, int pin, mailbox_s *mb) {
  ctx->timer = timer;
  ctx->period = period;
  ctx->pin = pin;
  ctx->mb = mb;
  pinMode(pin, INPUT);
}

// WiFi
void setup_wifi() {
  Serial.print("Connecting WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  wifiConnecting = true;
  connectStartTime = millis();
}

// MQTT
void setup_mqtt() {
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(mqtt_callback);
}

// Loop OLED
void loop_oled(struct oled_s *ctx, int wifiStatus, String ip = "", int statusCode = -1) {
  if (!waitFor(ctx->timer, ctx->period)) return;
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  if (wifiStatus == 0) display.println("WiFi: OFF");
  else if (wifiStatus == 1) display.println("WiFi: Connecting...");
  else if (wifiStatus == 2) {
    display.println("WiFi: OK");
    display.setCursor(0, 10);
    display.print("IP: ");
    display.println(ip);
  }

  display.setCursor(0, 30);
  display.print("Msg: ");
  display.println(messageEcran.substring(0, 16));
  display.setCursor(0, 45);
  display.print("LED: ");
  display.println(ctx->mb->state ? "ON" : "OFF");
  display.display();
}

// Loop LED
void loop_led(struct led_s *ctx) {
  digitalWrite(ctx->pin, ctx->mb->state);
}

// Loop photo
void loop_photo(struct Photo_s *ctx) {
  if (!waitFor(ctx->timer, ctx->period)) return;
  ctx->mb->value = analogRead(ctx->pin);
}

// WiFi loop
void loop_wifi() {
  if (!wifiConnected && !wifiConnecting) setup_wifi();

  if (wifiConnecting && WiFi.status() == WL_CONNECTED) {
    wifiIP = WiFi.localIP().toString();
    wifiConnected = true;
    wifiConnecting = false;
    Mailbox1.wifi_valid = true;
    Serial.print("WiFi OK, IP: ");
    Serial.println(wifiIP);
  }

  if (wifiConnecting && (millis() - connectStartTime > 10000)) {
    wifiConnecting = false;
    WiFi.disconnect();
    Mailbox1.wifi_valid = false;
    Serial.println("WiFi Timeout");
  }

  int wifiStatus = wifiConnected ? 2 : (wifiConnecting ? 1 : 0);
  loop_oled(&oled1, wifiStatus, wifiIP, WiFi.status());
}

// Loop MQTT
void loop_mqtt() {
  if (!mqttClient.connected()) {
    if (mqttClient.connect("ESP32Client")) {
      mqttClient.subscribe(topic_led);
      mqttClient.subscribe("ecran");
      Serial.println("MQTT connected & subscribed to 'led' and 'ecran'");
    } else {
      Serial.println("MQTT failed");
    }
  }
  mqttClient.loop();

  // Publish photoresistor value every 1s
  if (Mailbox1.wifi_valid && mqttClient.connected() && millis() - lastPublish > 1000) {
    String msg = "Light: " + String(Mailbox1.value);
    mqttClient.publish(topic_photo, msg.c_str());
    Serial.print("[MQTT] 发布 → ");
    Serial.println(msg);
    lastPublish = millis();
  }
}

// Setup
void setup() {
  Serial.begin(115200);
  setup_oled(&oled1, 0, 1000000, &Mailbox1);
  setup_LED(&led1, LED_PIN, &Mailbox1);
  setup_photo(&photo1, 2, 500000, ADC_PIN, &Mailbox1);
  setup_mqtt();
}

// Main loop
void loop() {
  loop_photo(&photo1);
  loop_led(&led1);
  loop_wifi();
  loop_mqtt();
}
