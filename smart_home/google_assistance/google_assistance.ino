#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <Servo.h>
#include <SoftwareSerial.h>

#define KIT_WIN_CTRL  D0
#define KIT_LIG_CTRL  D1
#define KIT_FAN_CTRL  D2
#define LIV_LIG_CTRL  D3
#define LIV_FAN_CTRL  D4
#define BED_WIN_CTRL  D5
#define BED_LIG_CTRL  D6
#define BED_FAN_CTRL  D7

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
Adafruit_MQTT_Subscribe denphongngu = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/denphongngu");
Adafruit_MQTT_Subscribe quatphongngu = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/quatphongngu");
Adafruit_MQTT_Subscribe cuasophongngu = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/cuasophongngu");
Adafruit_MQTT_Subscribe denphongbep = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/denphongbep");
Adafruit_MQTT_Subscribe quatphongbep = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/quatphongbep");
Adafruit_MQTT_Subscribe cuasophongbep = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/cuasophongbep");
Adafruit_MQTT_Subscribe denphongkhach = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/denphongkhach");
Adafruit_MQTT_Subscribe quatphongkhach = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/quatphongkhach");

Servo servo_windown;

void setup()
{
  pinMode(KIT_WIN_CTRL, OUTPUT);
  pinMode(KIT_LIG_CTRL, OUTPUT);
  pinMode(KIT_FAN_CTRL, OUTPUT);
  pinMode(BED_WIN_CTRL, OUTPUT);
  pinMode(BED_LIG_CTRL, OUTPUT);
  pinMode(BED_FAN_CTRL, OUTPUT);
  pinMode(LIV_LIG_CTRL, OUTPUT);
  pinMode(LIV_FAN_CTRL, OUTPUT);

  Serial.begin(9600);
  //Connect to WiFi
  Serial.print("\n\nConnecting Wifi... ");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  Serial.println("OK!");
  //Subscribe to the onoff feed
  mqtt.subscribe(&denphongngu);
  mqtt.subscribe(&quatphongngu);
  mqtt.subscribe(&cuasophongngu);
  mqtt.subscribe(&denphongbep);
  mqtt.subscribe(&quatphongbep);
  mqtt.subscribe(&cuasophongbep);
  mqtt.subscribe(&denphongkhach);
  mqtt.subscribe(&quatphongkhach);
  digitalWrite(KIT_WIN_CTRL, LOW);
  digitalWrite(KIT_LIG_CTRL, LOW);
  digitalWrite(KIT_FAN_CTRL, LOW);
  digitalWrite(BED_WIN_CTRL, LOW);
  digitalWrite(BED_LIG_CTRL, LOW);
  digitalWrite(BED_FAN_CTRL, LOW);
  digitalWrite(LIV_LIG_CTRL, LOW);
  digitalWrite(LIV_FAN_CTRL, LOW);
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
    if (subscription == &denphongngu)
    {
      DKdenphongngu();
    }

    if (subscription == &quatphongngu)
    {
      DKquatphongngu();
    }

    if (subscription == &cuasophongngu)
    {
      DKcuasophongngu();
    }

    if (subscription == &denphongbep)
    {
      DKdenphongbep();
    }

    if (subscription == &quatphongbep)
    {
      DKquatphongbep();
    }

    if (subscription == &cuasophongbep)
    {
      DKcuasophongbep();
    }
    if (subscription == &denphongkhach)
    {
      DKdenphongkhach();
    }

    if (subscription == &quatphongkhach)
    {
      DKquatphongkhach();
    }

  }

  // ping the server to keep the mqtt connection alive
  if (!mqtt.ping())
  {
    mqtt.disconnect();
  }
}

void DKdenphongngu()
{
  Serial.print("denphongngu: ");
  Serial.println((char*) denphongngu.lastread);
  
  if (strcmp((char*) denphongngu.lastread, "ON") == 0)
  {
    digitalWrite(BED_LIG_CTRL, HIGH);
  }
  else if (strcmp((char*) denphongngu.lastread, "OFF") == 0)
  {
    digitalWrite(BED_LIG_CTRL, LOW);
  }
}

void DKquatphongngu()
{ 
  Serial.print("quatphongngu: ");
  Serial.println((char*) quatphongngu.lastread);
  
  if (strcmp((char*) quatphongngu.lastread, "ON") == 0)
  {
    digitalWrite(BED_FAN_CTRL, HIGH);
  }
  else if (strcmp((char*) quatphongngu.lastread, "OFF") == 0)
  {
    digitalWrite(BED_FAN_CTRL, LOW);
  }
}

void DKcuasophongngu()
{ 
  Serial.print("cuasophongngu: ");
  Serial.println((char*) cuasophongngu.lastread);
  
  if (strcmp((char*) cuasophongngu.lastread, "OPEN") == 0)
  {
    Serial.println("cuasophongngu: OPEN");
    digitalWrite(BED_WIN_CTRL, HIGH);
  }
  else
  {
    Serial.println("cuasophongngu: CLOSE");
    digitalWrite(BED_WIN_CTRL, LOW);
  }
}

void DKdenphongbep()
{ 
  Serial.print("denphongbep: ");
  Serial.println((char*) denphongbep.lastread);
  
  if (strcmp((char*) denphongbep.lastread, "ON") == 0)
  {
    digitalWrite(KIT_LIG_CTRL, HIGH);
  }
  else if (strcmp((char*) denphongbep.lastread, "OFF") == 0)
  {
    digitalWrite(KIT_LIG_CTRL, LOW);
  }
}

void DKquatphongbep()
{ 
  Serial.print("quatphongbep: ");
  Serial.println((char*) quatphongbep.lastread);
  
  if (strcmp((char*) quatphongbep.lastread, "ON") == 0)
  {
    digitalWrite(KIT_FAN_CTRL, HIGH);
  }
  else if (strcmp((char*) quatphongbep.lastread, "OFF") == 0)
  {
    digitalWrite(KIT_FAN_CTRL, LOW);
  }
}
void DKcuasophongbep()
{ 
  Serial.print("cuasophongbep: ");
  Serial.println((char*) cuasophongbep.lastread);
  
  if (strcmp((char*) cuasophongbep.lastread, "OPEN") == 0)
  {
    digitalWrite(KIT_WIN_CTRL, HIGH);
  }
  else if (strcmp((char*) cuasophongbep.lastread, "CLOSE") == 0)
  {
    digitalWrite(KIT_WIN_CTRL, LOW);
  }
}
void DKdenphongkhach()
{ 
  Serial.print("denphongkhach: ");
  Serial.println((char*) denphongkhach.lastread);
  
  if (strcmp((char*) denphongkhach.lastread, "ON") == 0)
  {
    digitalWrite(LIV_LIG_CTRL, HIGH);
  }
  else if (strcmp((char*) denphongkhach.lastread, "OFF") == 0)
  {
    digitalWrite(LIV_LIG_CTRL, LOW);
  }
}

void DKquatphongkhach()
{ 
  Serial.print("quatphongkhach: ");
  Serial.println((char*) quatphongkhach.lastread);
  
  if (strcmp((char*) quatphongkhach.lastread, "ON") == 0)
  {
    digitalWrite(LIV_FAN_CTRL, HIGH);
  }
  else if (strcmp((char*) quatphongkhach.lastread, "OFF") == 0)
  {
    digitalWrite(LIV_FAN_CTRL, LOW);
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
