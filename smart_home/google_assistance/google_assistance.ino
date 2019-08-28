#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <Servo.h>

#define WINDOWN_PIN   4
#define FAN_PIN       0
#define LIGHT_PIN     2

#define WIFI_SSID "Sieu nhan gao"
#define WIFI_PASS "igategw020"
#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
#define MQTT_NAME "hieulucifer"
#define MQTT_PASS "fe9da9da57174e28a85b033455e0fe91"
 
//Set up MQTT and WiFi clients
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);
 
//Set up the feed you're subscribing to
Adafruit_MQTT_Subscribe den = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/den");
Adafruit_MQTT_Subscribe quat = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/quat");
Adafruit_MQTT_Subscribe cuaso = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/cuaso");

Servo servo_windown;

void setup()
{
  Serial.begin(9600);
  //Connect to WiFi
  servo_windown.attach(WINDOWN_PIN);
  Serial.print("\n\nConnecting Wifi... ");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
 
  Serial.println("OK!");
  //Subscribe to the onoff feed
  mqtt.subscribe(&den);
  mqtt.subscribe(&quat);
  mqtt.subscribe(&cuaso);
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  pinMode(FAN_PIN, OUTPUT);
  pinMode(LIGHT_PIN, OUTPUT);
  
  close_windown();
  
  digitalWrite(FAN_PIN, HIGH);
  digitalWrite(LIGHT_PIN, HIGH);
}
 
void loop()
{
  MQTT_connect();
  //Read from our subscription queue until we run out, or
  //wait up to 5 seconds for subscription to update
  Adafruit_MQTT_Subscribe * subscription;
  while ((subscription = mqtt.readSubscription(5000)))
  {
    //If we're in here, a subscription updated...
    if (subscription == &den)
    {
      DKden();
    }

    if (subscription == &quat)
    {
     DKquat();
    }

    if (subscription == &cuaso)
    {
      DKcuaso();
    }
  }
 
  // ping the server to keep the mqtt connection alive
  if (!mqtt.ping())
  {
    mqtt.disconnect();
  }
}

void DKden()
{
  //Print the new value to the serial monitor
      Serial.print("den: ");
      Serial.println((char*) den.lastread);
     //If the new value is  "ON", turn the light on.
     //Otherwise, turn it off.
      if (strcmp((char*) den.lastread, "ON") == 0)
      {
        Serial.println("Turn on the light");
        digitalWrite(LIGHT_PIN, LOW);
        
      }
      else if (strcmp((char*) den.lastread, "OFF") == 0)
      {
        Serial.println("Turn off the light");
        digitalWrite(LIGHT_PIN, HIGH);
      }
  }

void DKquat()
{
   //Print the new value to the serial monitor
      Serial.print("quat: ");
      Serial.println((char*) quat.lastread);
     //If the new value is  "ON", turn the light on.
     //Otherwise, turn it off.
      if (strcmp((char*) quat.lastread, "ON") == 0)
      {
        
        Serial.println("Turn on the fan");
        digitalWrite(FAN_PIN, LOW);
        
      }
      else if (strcmp((char*) quat.lastread, "OFF") == 0)
      {
        Serial.println("Turn off the fan");
        digitalWrite(FAN_PIN, HIGH);
        
      }
  }

void DKcuaso()
{
  //Print the new value to the serial monitor
      Serial.print("cuaso: ");
      Serial.println((char*) cuaso.lastread);
     //If the new value is  "ON", turn the light on.
     //Otherwise, turn it off.
      if (strcmp((char*) cuaso.lastread, "OPEN") == 0)
      {
        Serial.println("Open the windown");
        open_windown();
      }
      else if (strcmp((char*) cuaso.lastread, "CLOSE") == 0)
      {
        Serial.println("Close the windown");
        close_windown();
      }
  }  
 
void MQTT_connect()
{
  int8_t ret;
  // Stop if already connected.
  if (mqtt.connected())
  {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) // connect will return 0 for connected
  {
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0)
       {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
} //END CODE

void open_windown()
{
  int pos = 0;
  int i;

  for (i = 0; i < 100; i++)
  {
    servo_windown.write(pos);
    pos++;
    delay(50);
  }
}

void close_windown()
{
  int pos = 100;
  int i;

  for (i = 0; i < 100 ; i++)
  {
    servo_windown.write(pos);
    pos--;
    delay(50);
  }
}
