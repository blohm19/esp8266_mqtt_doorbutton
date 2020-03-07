#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <EEPROM.h>


/************************* Manual Config *********************************/

const char* ssid = "....";
const char* password = "....";

char* mqtt_server = "mqtt.home";
int mqtt_server_port = 1883;

String purpose = "doorbell"; // mqtt topic becomes debug/doorbell/mqttClientID

String code_version = "180603-1.0";


/************************* Power *********************************/

int vcc;
ADC_MODE(ADC_VCC);

/************************* WiFi Access Point *********************************/

String wifimac ="";
IPAddress ipaddress;

/************************* MQTT Server *********************************/
String mqttClientId = "";
const char* mqtt_user = "admin";
const char* mqtt_password = "admin";
String message = "";

String topicDebug = "";
String topicRing = "";

/************************* ESP8266 WiFiClient *********************************/
WiFiClient wifiClient;


/*************not used yet, for subscription of messages ******************************/
void mqtt_callback(char* topic, byte* payload, unsigned int length) {

 Serial.print("Message arrived [");
 Serial.print(topic);
 Serial.print("] ");
 for (int i = 0; i < length; i++) {
   Serial.print((char)payload[i]);
 }
 Serial.println();
}

/************************* MQTT client *********************************/
PubSubClient client(mqtt_server, mqtt_server_port, mqtt_callback, wifiClient );
   

long startMillis;

int iter = 0;

String getLowerMacAddress() {
  byte mac[6];
  WiFi.macAddress(mac);
  char  buff[10];
  String cMac = "";
  for (int i = 3; i < 6; ++i) {
    snprintf(buff, sizeof(buff), "%02X", mac[i]);
    cMac += buff;
    }
  cMac.toLowerCase();
  return cMac;
}

String getMacAddress() {
  byte mac[6];
  WiFi.macAddress(mac);
  char  buff[10];
  String cMac = "";
  for (int i = 0; i < 6; ++i) {
    snprintf(buff, sizeof(buff), "%02X", mac[i]);
    cMac += buff;
    }
  cMac.toLowerCase();
  return cMac;
}

String ipAddressToString(IPAddress address) {
 return String(address[0]) + "." + 
        String(address[1]) + "." + 
        String(address[2]) + "." + 
        String(address[3]);
}

/******* Utility function to connect or re-connect to MQTT-Server ********************/
void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {

  // Attempt to connect
  if (client.connect(mqttClientId.c_str())) {

    Serial.println("connected");

  } else {
    // Wait 5 seconds before retrying
    delay(5000);
  }
 }
}

/************* Setup ******************************/
void setup(void) {
 startMillis = millis();
 iter = 0;
 Serial.begin(115200);

 WiFi.mode(WIFI_STA);

 // Connect to WiFi network
 WiFi.begin(ssid, password);

 Serial.print("\n\r \n\rConnecting to ");
 Serial.println(ssid);

 // Wait for connection
 while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
 }

  wifimac = getLowerMacAddress();
  mqttClientId = "esp_" +wifimac; 
  ipaddress = WiFi.localIP();
  Serial.println ("Connected mqttClientId= " + mqttClientId);
 // Create String for MQTT Topics
 topicDebug = "debug/" +purpose+"/"+ mqttClientId;
 topicRing = "ring/" +purpose+"/"+ mqttClientId;

 startMillis = millis();
 Serial.print("\n\rIP address: ");
 Serial.println(WiFi.localIP());

 reconnect();
 vcc = ESP.getVcc();//readvdd33();
 String msg = "Client " + mqttClientId + " (" +getMacAddress() + ") with IP Address= " + ipAddressToString(WiFi.localIP()) + " " +code_version + " VCC= " + String(vcc) + " mV" ;
 
 client.publish(topicDebug.c_str(), msg.c_str());
 msg = "ring... VCC= " + String(vcc) + " mV";
 client.publish(topicRing.c_str(), msg.c_str() );     

 delay (500);

 ESP.deepSleep(0);
}


/************* Loop ******************************/
void loop(void) {
 // does never enter loop.
 
}
