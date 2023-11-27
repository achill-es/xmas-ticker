/*
 *  module to (re)connect to both WiFi and then MQTT broker
 */

#include <ESP8266WiFi.h>
#include <string.h>

#include "credentials.h"

#define WAITxSEC  (5000)
#define MQTT_QOS1 (1)

// init a secured WiFi client
static WiFiClient espClient;

// use WiFi client to init a MQTT client
PubSubClient mqttClient (espClient);

void reconnect (bool fromsetup) 
{
  if (fromsetup)
  {
    WiFi.mode (WIFI_STA);
    // set our MQTT broker
    mqttClient.setServer (mqtt_broker, mqtt_port);
  }

  // connecting to a WiFi network
  if (WiFi.status () != WL_CONNECTED)
  {
    do
    {
      delay (WAITxSEC);
      Serial.printf ("%sonnecting to WiFi %s...\n", (fromsetup ? "C" : "Rec"), wifi_ssid);
      WiFi.begin (wifi_ssid, wifi_password);
    } while (WiFi.status () != WL_CONNECTED);
    
    Serial.printf ("%sonnected to the WiFi network.\n", (fromsetup ? "C" : "Rec"));
    Serial.printf ("IP address: %s\n", WiFi.localIP().toString().c_str());
  }
  
  // connecting to MQTT broker
  while (!mqttClient.connected ())
  {
    char mqtt_clientid[mqtt_clen];

    sprintf (mqtt_clientid, "%s-%s", mqtt_prefix, WiFi.localIP().toString().c_str());
    Serial.printf ("Client %s %sconnects to the MQTT broker...\n", mqtt_clientid, fromsetup ? "" : "re");

    if (mqttClient.connect (mqtt_clientid))
    {
      Serial.printf ("%sonnected to MQTT broker.\n", (fromsetup ? "C" : "Rec"));
    }
    else
    {
      Serial.printf ("Failed to %sconnect to MQTT broker, rc = %d.\n", fromsetup ? "" : "re", mqttClient.state());
      Serial.println ("Retrying in a few seconds...");
      delay (WAITxSEC);
    }
  }

  // subscribe to our topics
  //mqttClient.subscribe (mqtt_topic_t, MQTT_QOS1);
  mqttClient.subscribe (mqtt_topic_t);
  mqttClient.subscribe (mqtt_topic_i);
  mqttClient.subscribe (mqtt_topic_s);
  Serial.println ("Subscribed to all our topics.");
}

// end of code file