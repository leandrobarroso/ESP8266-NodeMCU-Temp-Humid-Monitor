//----------------------------------------------------------------------------------------------------------
//
//
//IOT_TEMP-HUMID.ino - gets temperature and humidity data and publish on internet through MQTT service  
//                
//
//Author: Leandro Barroso <lrbarroso84@hotmail.com>
//
//Version History:
//
//   v1.0 2021-03-18, Leandro Barroso
//       - initial version
//
//License: MIT No Attribution License
//
//----------------------------------------------------------------------------------------------------------

// *** INPUT/OUTPUT INITIALIZING ***
// --- DHT ---
#include <DHT.h>
#define DHTPIN D3
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
float temperature;
float humidity;

// --- DISPLAY ---
#include <Wire.h>
#include "SSD1306Wire.h"
SSD1306Wire display(0x3c, SDA, SCL);

// --- WIFI ---
#include <ESP8266WiFi.h>
WiFiClient nodemcuClient;
const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";

// --- MQTT ---
#include <PubSubClient.h>
PubSubClient client(nodemcuClient);
const char* mqtt_Broker = "MQTT_BROKER";
const char* mqtt_ClientID = "MQTT_CLIENT";
const char* topicTemperature = "MQTT_TOPIC_FOR_TEMPERATURE";
const char* topicHumidity = "MQTT_TOPIC_FOR_HUMIDITY";

// *** MAIN FUNCTIONS ***
void setup() {
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Initializing Setup...");
  
  // --- DHT ---
  Serial.println("Initializing DHT Sensor...");
  dht.begin();
  Serial.println("DHT Sensor OK!!!");

  // --- DISPLAY ---
  Serial.println("Initializing OLED Display...");
  configDisplay();
  Serial.println("OLED Display OK!!!");
  displayMessage("Display iniciado!");

  // --- WIFI ---
  Serial.print("Connecting ");
  displayMessage("Conectando...");
  connectWifi();
  Serial.println("Connected WIFI!!!");
  displayMessage("WIFI Conectado!!!");

  // --- MQTT ---
  Serial.println("Initializing MQTT service...");  
  client.setServer(mqtt_Broker, 1883);
  Serial.println("MQTT service OK!!!");
}

void loop() {
  reconnectWIFI();
  reconnectMQTT();
  updateTemperatureHumidity();
  printTemperatureHumidity();
  publishTemperatureHumidity();
  displayData("Temperatura:", String(temperature).c_str(), "ºC");
  delay(2000);
  displayData("Umidade:", String(humidity).c_str(), "%");
  delay(2000);
}

// *** AUX FUNCTIONS ***
// --- DHT ---
void updateTemperatureHumidity() {
  temperature = dht.readTemperature(false);
  humidity = dht.readHumidity();
}

void printTemperatureHumidity() {
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("°C");
  Serial.print("   ");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");
}

// --- DISPLAY ---
void configDisplay() {
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.clear();
}

void displayData(String unity, String value, String symbol) {
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, unity);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_24);
  display.drawString(60, 20, value);
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(110, 20, symbol);
  display.display();
}

void displayMessage(String text){
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 20, text);
  display.display();
}

// --- WIFI ---
void connectWifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(".");
}

void reconnectWIFI() {
  while (WiFi.status() != WL_CONNECTED) {
    connectWifi();
  }
}

// --- MQTT ---
void reconnectMQTT() {
  while (!client.connected()) {
    client.connect(mqtt_ClientID);
  }
}

void publishTemperatureHumidity() {
  client.publish(topicTemperature, String(temperature).c_str(), true);
  client.publish(topicHumidity, String(humidity).c_str(), true);
}

//Possible fails and improvement sugestions 
//TODO: include output language options
//TODO: use icons on display for some design improvents
