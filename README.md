# EzThermometer.Device
Connected Thermometer

## Description
This sensor displays on its OLED screen and publishes on MQTT the following information :
- temperature
- humidity
- CO2 concentration

![Device](/device.jpg)

## Disclaimer
I am not an expert in embedded system programming and C ++. This project was created for personal use and shared for educational purposes.

## Hardware
- ESP32 NodeMCU Module WLAN WiFi Dev Kit C Development Board
- DHT22 / AM2302 Temperature and humidity sensor
- MH-Z19C NDIR CO2 Sensor
- AZDelivery AZ-Touch Smart Home Kit de Montage Mural avec Écran Tactile TFT LCD de 2,4 Pouces, contrôleur Tactile SPI XPT2046

### Assembly
![Assembly](/assembly.png)

### ArduiTouch Connection Diagram
![ArduiTouch Connection Diagram](/arduitouch-connection-diagram.png)

|DHT22|ESP32|
|------|---|
|+|3.3V|
|Out|IO32|
|-|GND|

|MH-Z19C|ESP32|
|------|---|
|VCC|5V|
|GND|GND|
|TX| RX (IO16)|
|RX| TX (IO17)|

## Getting Started
### Prerequisites
- Visual Studio Code
- PlatformIO IDE for VSCode

### Software Configuration
Before uploading this program to your MCU, you need to set following configuration variables :
#### WiFi
- WIFI_SSID
- WIFI_PASSWORD

#### MQTT
- MQTT_CLIENT_ID
- MQTT_SERVER_IP
- MQTT_SERVER_PORT
- MQTT_USER
- MQTT_PASSWORD

- humidityMqttTopic
- temperatureMqttTopic
- co2MqttTopic

## Authors
* **Amael BERTEAU**