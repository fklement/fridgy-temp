/*
 Fridgy MQTT temperature with ESP8266 and DS18b20
 by Felix Klement
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS D7
#define BUILTIN_LED 2
#define mqtt_server     "ip-address-of-your-mqtt-broker"
#define mqtt_user       "mqtt-user"
#define mqtt_password   "password-of-your-mqtt-user"
#define temp_topic      "your-sensor-topic"
#define wifi_SSID       "your-wifi-ssid"
#define wifi_password   "your-wifi-password"

long lastPub = 0;
float celcius = 0;

WiFiClient espClient;
PubSubClient client(espClient);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensor(&oneWire);

void setup(void)
{
  pinMode(BUILTIN_LED, OUTPUT);  
  
  WiFi.begin(wifi_SSID, wifi_password);

  // Let the led blink while connecting to wifi
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(BUILTIN_LED, LOW); 
    delay(200);
    digitalWrite(BUILTIN_LED, HIGH);
    delay(200);
  }

  // Station (networking)
  WiFi.mode(WIFI_STA);  
  
  sensor.begin();
  client.setServer(mqtt_server, 1883);
}

void loop(void)
{ 
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop();

  long now = millis();
  // Check if the last pub is older than 30 secs
  if (now - lastPub > 30000) {
    lastPub = now;

    // Get the current temperature from the DS18b20 sensor
    sensor.requestTemperatures(); 
    celcius=sensor.getTempCByIndex(0);

    // Publish the temperature to our topic
    client.publish(temp_topic, String(celcius).c_str(), true);
  }
}

// Function for connecting to our MQTT-Broker
void connectMQTT() {
  // Loop until we're connected
  while (!client.connected()) {
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
       // Let the builtin LED flash for 3 seconds when we're connected
       digitalWrite(BUILTIN_LED, LOW); 
       delay(3000);                   
       digitalWrite(BUILTIN_LED, HIGH);
    } else {
      delay(5000);
    }
  }
}


