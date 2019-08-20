#include <dht.h>
#include <Keypad.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>	
#include <Servo.h>
#include <Scheduler.h>


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
#define NO_OF_ROW				4		  // Using for keypad	
#define NO_OF_COL				3		  // Using for keypad	

MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo servo_windown;
LiquidCrystal_I2C lcd(0x27,16,2);
dht DHT_KIT;
dht DHT_LIV;

byte rowPins[ROWS] = {A8, A9, A10, A11}; //connect to the row pinouts of the kpd
byte colPins[COLS] = {A12, A13, A14}; //connect to the column pinouts of the kpd

char keys[NO_OF_ROW][NO_OF_COL] = {
{'1','2','3'},
{'4','5','6'},
{'7','8','9'},
{'*','0','#'}
};

char pass[4] = {'1', '2', '3', '4'};
String card_id;


Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );



void setup()
{
  	Serial.begin(9600);

 	pinMode(LIGHT_SS_PIN, INPUT);
 	pinMode(FAN_KIT_PIN, OUTPUT);
 	pinMode(LIG_KIT_PIN, OUTPUT);
 	pinMode(LIG_LIV_PIN, OUTPUT);
 	pinMode(OPEN_DOOR, OUTPUT);
 	pinMode(CLOSE_DOOR, OUTPUT);
 	pinMode(BUZZER_GAS, OUTPUT);
 	servo_windown.attach(WD_RC_SERVO);
 	lcd.init();                      // initialize the lcd 
 
  	// Print a message to the LCD.
  	lcd.backlight();
  	lcd.print("Hello");
  	digitalWrite(BUZZER_GAS, HIGH);

 	Scheduler.startLoop(kit_loop);
 	Scheduler.startLoop(liv_loop);
 	delay(1000);
}

/*this loop will use for close or open door*/
void loop()
{
// 	int chk = DHT.read11(DHT11_PIN);
// 	int temp = DHT.temperature;
// 	int gas = analogRead(GAS_SS_PIN);

// 	Serial.print("GAS = ");
// 	Serial.println(gas); //doc data muc bao dong 500 la co leakage gas
// //  Serial.print("Temperature = ");
// //  Serial.println(temp);//doc data temp is 30 la bat quat
// //  Serial.print("LUX = ");
// //  Serial.println(digitalRead(LIGHT_SS_PIN));//sang input is 0, toi input is 1
// 	delay(500);
	

	lcd.clear();
	delay(100);
	lcd.setCursor(0, 1);
	lcd.print("* for RFID");
	lcd.setCursor(1, 1);
	lcd.print("# for pass");

	key = kpd.getKey();
	if('*' == key)
	{
		enter_pass();
	}
	else if('#' == key)
	{
		scan_card();
	}
}

void kit_loop()
{
	int gas = analogRead(GAS_SS_PIN);
	int chk = DHT_KIT.read11(DHT11_SS_2_PIN);
	int temp = DHT_KIT.temperature;
	int light = digitalRead(LIGHT_SS_PIN);

	if(gas >= 500)
	{
		while(gas >= 500)
		{
			gas_warning();
			turn_on_kit_fan();
			turn_off_kit_ligh();
			gas = analogRead(GAS_SS_PIN);
		}
	}

	if(30 <= temp)
	{
		turn_on_kit_fan();
	}
	else
	{
		turn_off_kit_fan();
	}

	if(light == 0)
	{
		turn_on_kit_ligh();
	}
	else
	{
		turn_off_kit_ligh();
	}
}

void liv_loop()
{
	int chk = DHT_LIV.read11(DHT11_SS_1_PIN);
	int temp = DHT_LIV.temperature;
	int light = digitalRead(LIGHT_SS_PIN);

	if(30 <= temp)
	{
		turn_on_liv_fan();
	}
	else
	{
		turn_off_liv_fan();
	}

	if(light == 0)
	{
		turn_on_liv_ligh();
	}
	else
	{
		turn_off_liv_ligh();
	}
}

void scan_card()
{
	String card;
	int time;
	lcd.clear();
	delay(100);
	lcd.setCursor(0, 1);
	lcd.print("CARD PLEASE");
	time = 0;
	while(time < 2000)
	{
		if(mfrc522.PICC_IsNewCardPresent())
		{
			if(mfrc522.PICC_ReadCardSerial())
			{
				break;
			}
		}
		time++;
		delay(10)
	}

	if(time < 2000)
	{
		int i;
		for(i = 0; i < mfrc522.uid.size; i++)
		{
			card.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
 			card.concat(String(mfrc522.uid.uidByte[i], HEX));
		}

		if(card.substring(1) == card_id.substring(1))
		{
			open_door();
			delay(1000);
			stop_door();
			delay(3000);
			close_door();
			delay(1000);
			stop_door();
		}
		else
		{
			lcd.clear();
			delay(100);
			lcd.setCursor(0, 1);
			lcd.print("WRONG CARD");
			door_warning();
		}
	}
	else
	{
		lcd.clear();
		delay(100);
		lcd.setCursor(0, 1);
		lcd.print("TIMEOUT");
	}
}

void enter_pass()
{
	char pass_enter[4];
	char key = 0;
	int count = 0;
	int i;
	lcd.clear();
	delay(100);
	lcd.setCursor(0, 1);
	lcd.print("ENTER PASS");
	while(i < 300)
	{
		key = kpd.getKey();
		if(key)
		{
			pass_enter[count] = key;
			i = 0;
			count++;
		}
		if(count == 4)
		{
			break;
		}
		i++;
		delay(10);
	}

	if(count < 4)
	{
		lcd.clear();
		delay(100);
		lcd.setCursor(0, 1);
		lcd.print("TIMEOUT");
	}
	else
	{
		if(!memcmp(pass, pass_enter, 4))
		{
			open_door();
			delay(1000);
			stop_door();
			delay(3000);
			close_door();
			delay(1000);
			stop_door();
		}
		else
		{
			lcd.clear();
			delay(100);
			lcd.setCursor(0, 1);
			lcd.print("Wrong Password");
			door_warning();
		}
	}
}

void turn_on_liv_fan()
{
	digitalWrite(FAN_LIV_PIN, HIGH);
}

void turn_off_liv_fan()
{
	digitalWrite(FAN_LIV_PIN, LOW);
}

void turn_on_kit_fan()
{
	digitalWrite(FAN_KIT_PIN, HIGH);
}

void turn_off_kit_fan()
{
	digitalWrite(FAN_KIT_PIN, LOW);
}

void turn_on_liv_light()
{
	digitalWrite(LIG_LIV_PIN, HIGH);
}

void turn_off_liv_light()
{
	digitalWrite(LIG_LIV_PIN, LOW);
}

void turn_on_kit_ligh()
{
	digitalWrite(LIG_KIT_PIN, HIGH);
}

void turn_off_kit_ligh()
{
	digitalWrite(LIG_KIT_PIN, LOW);
}

void open_door()
{
	digitalWrite(OPEN_DOOR, HIGH);
	digitalWrite(CLOSE_DOOR, LOW);
}

void stop_door()
{
	digitalWrite(OPEN_DOOR, LOW);
	digitalWrite(CLOSE_DOOR, LOW);
}

void close_door()
{
	digitalWrite(OPEN_DOOR, LOW);
	digitalWrite(CLOSE_DOOR, HIGH);
}

void open_windown()
{
	int pos = 0;

	for(i = 0; i < 90; i++)
	{
		servo_windown.write(pos);
		pos++;
		delay(50);
	}
}

void close_windown()
{
	int pos = 90;
	int i;

	for(i = 0; i < 90; i++)
	{
		servo_windown.write(pos);
		pos--;
		delay(50);
	}
}

void gas_warning()
{
	int i;

	for(i = 0; i < 20; i++)
	{
		digitalWrite(BUZZER_GAS, LOW);
		delay(100);
		digitalWrite(BUZZER_GAS, HIGH)
		delay(50);
	}
}

void door_warning()
{
	int i;

	for(i = 0; i < 10; i++)
	{
		digitalWrite(BUZZER_GAS, LOW);
		delay(500);
		digitalWrite(BUZZER_GAS, HIGH)
		delay(100);
	}
}