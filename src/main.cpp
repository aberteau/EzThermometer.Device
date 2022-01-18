#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <PeriodicTask.h>
#include <MHZ19.h>
#include <TFT_eSPI.h>
#include "xbm.h"

#define MQTT_VERSION MQTT_VERSION_3_1_1

#define RXD2 16
#define TXD2 17
#define MHZ19_BAUDRATE 9600
#define MHZ19_PROTOCOL SERIAL_8N1

TFT_eSPI dsply = TFT_eSPI(); // TFT LCD screen object

MHZ19 mhz19;

// Device Id
const String deviceId = "";

// Wifi: SSID and password
const char* WIFI_SSID = "";
const char* WIFI_PASSWORD = "";

// MQTT: ID, server IP, port, username and password
const PROGMEM char* MQTT_CLIENT_ID = "";
const PROGMEM char* MQTT_SERVER_IP = "";
const PROGMEM uint16_t MQTT_SERVER_PORT = 1883;
const PROGMEM char* MQTT_USER = "";
const PROGMEM char* MQTT_PASSWORD = "";

const String deviceMqttTopicBase = "sensor/" + deviceId;
const String humidityMqttTopic = deviceMqttTopicBase + "/humidity";
const String temperatureMqttTopic = deviceMqttTopicBase + "/temperature";
const String co2MqttTopic = deviceMqttTopicBase + "/co2";

float humidity;
String humidityStr;

float temperature;
String temperatureStr;

int co2;
String co2Str;

const unsigned long mqttPublishPeriodInSeconds = 30;

// sleeping time
const PROGMEM uint16_t SLEEPING_TIME_IN_SECONDS = 20;

// DHT - GPIO32
#define DHTPIN 32
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
WiFiClient wifiClient;
PubSubClient client(wifiClient);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("INFO: Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("INFO: connected");
    } else {
      Serial.print("ERROR: failed, rc=");
      Serial.print(client.state());
      Serial.println("DEBUG: try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// function called to publish the temperature and the humidity
void mqttPublish(String topic, float t) {
  if (!client.connected()) {
    reconnect();
  }

  int topicCharArraySize = topic.length()+1;
  char topicCharArray[topicCharArraySize];
  topic.toCharArray(topicCharArray, topicCharArraySize);
  
  char payloadCharArray[50];
  String payloadStr = String(t);
  int payloadCharArraySize = payloadStr.length() + 1;
  payloadStr.toCharArray(payloadCharArray, payloadCharArraySize);
  
  client.publish(topicCharArray, payloadCharArray);
  Serial.println("Published on " + topic);
}

void mqttPublish() {
  mqttPublish(humidityMqttTopic, humidity);
  mqttPublish(temperatureMqttTopic, temperature);
  mqttPublish(co2MqttTopic, co2);
}

void setupWiFi(){
  // init the WiFi connection
  Serial.println();
  Serial.println();
  Serial.print("INFO: Connecting to ");
  WiFi.mode(WIFI_STA);
  Serial.println(WIFI_SSID);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("INFO: WiFi connected");
  Serial.println("INFO: IP address: ");
  Serial.println(WiFi.localIP());
}

void setupMqtt(){
  client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
}

void setupDisplay() {
  // turn ON the dispaly
  pinMode(15, OUTPUT);    // Backlight pin of the display is connecte to this pin of ESP32
  digitalWrite(15, LOW);  // we have to drive this pin LOW in order to turn ON the display
  delay(1);               // delay for 1ms

  dsply.init();
  dsply.fillScreen(TFT_BLACK);    //  fill the screen with black color
  dsply.setTextColor(TFT_WHITE);  //  set the text color
  dsply.setRotation(1);
}

String getHumidityString() {
    String str = String(humidity, 1);
    str.concat(" %");
    return str;
}

String getTemperatureString() {
    String str = String(temperature, 1);
    str.concat(" *C");
    return str;
}

String getCO2String() {
    String str = String(co2);
    str.concat(" ppm");
    return str;
}

void readDHT22() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("ERROR: Failed to read from DHT sensor!");
  } else {
    humidity = h;
    temperature = t;
    
    humidityStr = getHumidityString();
    Serial.println(humidityStr);

    temperatureStr = getTemperatureString();
    Serial.println(temperatureStr);
  }
}

uint32_t getCo2Color(){
  if (co2 < 800) {
    return TFT_GREEN;
  }
  
  if (co2 < 1500) {
    return TFT_ORANGE;
  }
  return TFT_RED;
}

void readMHZ19() {
  co2 = mhz19.getCO2();
  co2Str = getCO2String();
  Serial.println(co2Str);
}

void refreshDisplay() {
  dsply.setTextSize(3);

  dsply.fillRect(100, 10, 340, 64, TFT_BLACK);
  dsply.setCursor(100, 30);
  dsply.println(temperatureStr);

  dsply.fillRect(100, 90, 340, 64, TFT_BLACK);
  dsply.setCursor(100, 110);
  dsply.println(humidityStr);

  dsply.fillRect(100, 170, 340, 64, TFT_BLACK);
  dsply.setCursor(100, 190);
  dsply.println(co2Str);

  uint32_t color = getCo2Color();
  dsply.fillRect(260, 170, 64, 64, color);
}

void showLayout() {
  dsply.pushImage(20, 10, image_width, image_width, thermometerIcon);
  dsply.pushImage(20, 90, image_width, image_width, humidityIcon);
  dsply.pushImage(20, 170, image_width, image_width, co2Icon);
}

PeriodicTask readDHT22PeriodicTask(2000UL, readDHT22);

PeriodicTask readMHZ19PeriodicTask(1000UL, readMHZ19);

PeriodicTask mqttPublishPeriodicTask(mqttPublishPeriodInSeconds * 1000UL, mqttPublish);

PeriodicTask refreshDisplayPeriodicTask(1000UL, refreshDisplay);

void setup() {
  // init the serial
  Serial.begin(9600);
  Serial.println("DeviceId: " + deviceId);
  dht.begin();

  setupWiFi();
  setupMqtt();
  setupDisplay();
  showLayout();

  Serial2.begin(MHZ19_BAUDRATE, MHZ19_PROTOCOL, RXD2, TXD2);

  mhz19.begin(Serial2);
  mhz19.autoCalibration(false);
}

void loop() {
  readDHT22PeriodicTask.loop();
  readMHZ19PeriodicTask.loop();

  refreshDisplayPeriodicTask.loop();

  mqttPublishPeriodicTask.loop();
}