#include <dht.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>	
#include <Servo.h>



/*
 * -----------------------------------
 *             MFRC522      Arduino   
 *             Reader/PCD   Mega      
 * Signal      Pin          Pin       
 * -----------------------------------
 * RST/Reset   RST          49        
 * SPI SS      SDA(SS)      53        
 * SPI MOSI    MOSI         51        
 * SPI MISO    MISO         50        
 * SPI SCK     SCK          52        
*/

#define RST_PIN         		49        // Using for RCC-522
#define SS_PIN          		53        // Using for RCC-522
#define DHT11_SS_1_PIN       	7		  // Using for DHT Livingroom
#define DHT11_SS_2_PIN			8		  // USING for DHT kitchen	
#define	WD_RC_SERVO				3		  // Using Control RC Servo windown	
#define LIGHT_SS_PIN    		2		  // Using like input read	
#define GAS_SS_PIN    			A0		  // USING for ADC	GAS SENSOR MQ-02
#define FAN_KIT_PIN				9		  // Control relay fan in kitchen
#define LIG_KIT_PIN				10		  // Control relay light in kitchen	
#define FAN_LIV_PIN				11		  // Control relay fan in Livingroom
#define LIG_LIV_PIN				12		  // Control relay light in Livingroom
#define OPEN_DOOR				5		  // Using to open main door
#define CLOSE_DOOR				6		  // Using to close main door
#define BUZZER_GAS				4		  // Using buzz waring for gas leakage	

MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo servo_windown;
LiquidCrystal_I2C lcd(0x27,16,2);
dht DHT;

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
