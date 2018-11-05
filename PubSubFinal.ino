#include <esp_wpa2.h>
#include <WiFi.h>
#include <PubSubClient.h>
 
const char* ssid = "NJITsecure";
#define EAP_IDENTITY "ar664"
#define EAP_PASSWORD "Something123"
#define MENOPC "ESP32 Board"
const char* mqttServer = "m15.cloudmqtt.com";
const int mqttPort = 11252;
const char* mqttUser = "eps32";
const char* mqttPassword = "something";

int SensorTemp;
int SensorLight;
int SensorMoisture;


int loopcount = 0;

char buf[4];

WiFiClient espClient;
PubSubClient client(espClient);
 
void setup() {
 
  Serial.begin(115200);

  pinMode(A18, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);

  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
  esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT();
  esp_wifi_sta_wpa2_ent_enable(&config);

  Serial.println("MAC address: ");
  Serial.println(WiFi.macAddress());
  WiFi.begin(ssid);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
 
  client.setServer(mqttServer, mqttPort);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP32", mqttUser, mqttPassword )) {
 
      Serial.println("connected");
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
 
  client.publish("Sensors/Temperature", "Hello from ESP32");
  client.subscribe("Sensors/Temperature");
  client.setCallback(callback);
}

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  Serial.println();
  Serial.println("-----------------------");

}



void loop() {
  client.loop();
  SensorTemp = analogRead(A18);
  Serial.print("Temperature: ");
  Serial.println(SensorTemp);
  delay(750);
  
  SensorLight = analogRead(A4);
  Serial.print("Light: ");
  Serial.println(SensorLight);
  delay(750);
  
  SensorMoisture = analogRead(A5);
  Serial.print("Moisture: ");
  Serial.println(SensorMoisture);
  delay(750);

  if(loopcount > 1000){
    sprintf(buf, "%03i", SensorTemp);
    client.publish("Sensors/Temperature", buf);
    sprintf(buf, "%03i", SensorLight);
    client.publish("Sensors/Light", buf);
    sprintf(buf, "%03i", SensorMoisture);
    client.publish("Sensors/Moisture", buf);
    loopcount = 0;
  }

  loopcount++;
  delay(250);
}
