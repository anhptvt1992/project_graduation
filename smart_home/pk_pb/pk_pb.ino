#include <dht.h>

dht DHT;

#define DHT11_PIN       7
//#define LIGHT_SS_PIN    A0
#define GAS_SS_PIN    A0

void setup(){
  Serial.begin(9600);
//  pinMode(LIGHT_SS_PIN, INPUT);
}

void loop()
{
  int chk = DHT.read11(DHT11_PIN);
  int temp = DHT.temperature;
  int gas = analogRead(GAS_SS_PIN);

  Serial.print("GAS = ");
  Serial.println(gas); //doc data muc bao dong 500 la co leakage gas
//  Serial.print("Temperature = ");
//  Serial.println(temp);//doc data temp is 30 la bat quat
//  Serial.print("LUX = ");
//  Serial.println(digitalRead(LIGHT_SS_PIN));//sang input is 0, toi input is 1
  delay(500);
}
