#include <dht.h>
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <TaskScheduler.h>

/*
   -----------------------------------
               MFRC522      Arduino
               Reader/PCD   Mega
   Signal      Pin          Pin
   -----------------------------------
   RST/Reset   RST          49
   SPI SS      SDA(SS)      53
   SPI MOSI    MOSI         51
   SPI MISO    MISO         50
   SPI SCK     SCK          52
*/

#define RST_PIN         		49        // Using for RCC-522
#define SS_PIN          		53        // Using for RCC-522
#define DHT11_SS_1_PIN       	7		  // Using for DHT Livingroom
#define DHT11_SS_2_PIN			8		  // USING for DHT kitchen	
#define	WD_RC_SERVO				3		  // Using Control RC Servo windown	
#define LIGHT_SS_PIN    		A5		  // Using like input read	
#define GAS_SS_PIN    			A0		  // USING for ADC	GAS SENSOR MQ-02
#define FAN_KIT_PIN				9		  // Control relay fan in kitchen
#define LIG_KIT_PIN				10		  // Control relay light in kitchen	
#define FAN_LIV_PIN				11		  // Control relay fan in Livingroom
#define LIG_LIV_PIN				12		  // Control relay light in Livingroom
#define OPEN_DOOR				6		  // Using to open main door
#define CLOSE_DOOR				5		  // Using to close main door
#define BUZZER_GAS				4		  // Using buzz waring for gas leakage	
#define NO_OF_ROW				4		  // Using for keypad	
#define NO_OF_COL				3		  // Using for keypad	

Scheduler ts;

void door_loop();
void kit_loop();
void liv_loop();


//Task t1 (100 * TASK_MILLISECOND, TASK_FOREVER, &door_loop, &ts, true);
//Task t2 (TASK_IMMEDIATE, TASK_FOREVER, &liv_loop, &ts, true);
//Task t3 (200 * TASK_MILLISECOND, TASK_FOREVER, &kit_loop, &ts, true);

MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo servo_windown;
LiquidCrystal_I2C lcd(0x27, 16, 2);
dht DHT_KIT;
dht DHT_LIV;

byte rowPins[NO_OF_ROW] = {A8, A9, A10, A11}; //connect to the row pinouts of the kpd
byte colPins[NO_OF_COL] = {A12, A13, A14}; //connect to the column pinouts of the kpd

char keys[NO_OF_ROW][NO_OF_COL] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

char pass[4] = {'1', '2', '3', '4'};
char card_id[4] = {0x7B, 0x9B, 0x7E, 0x22};


Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, NO_OF_ROW, NO_OF_COL );



void setup()
{
  Serial.begin(9600);

  //  pinMode(LIGHT_SS_PIN, INPUT);
  pinMode(FAN_KIT_PIN, OUTPUT);
  pinMode(FAN_LIV_PIN, OUTPUT);
  pinMode(LIG_KIT_PIN, OUTPUT);
  pinMode(LIG_LIV_PIN, OUTPUT);
  pinMode(OPEN_DOOR, OUTPUT);
  pinMode(CLOSE_DOOR, OUTPUT);
  pinMode(BUZZER_GAS, OUTPUT);
  servo_windown.attach(WD_RC_SERVO);
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();    // Init MFRC522
  lcd.init();                      // initialize the lcd
  // Print a message to the LCD.
  lcd.backlight();
  lcd.print("Hello");
  digitalWrite(FAN_KIT_PIN, HIGH);
  digitalWrite(LIG_KIT_PIN, HIGH);
  digitalWrite(LIG_LIV_PIN, HIGH);
  digitalWrite(OPEN_DOOR, LOW);
  digitalWrite(CLOSE_DOOR, LOW);
  digitalWrite(BUZZER_GAS, HIGH);


}

/*this loop will use for close or open door*/
void loop()
{
  //  ts.execute();
  //  door_loop();
  kit_loop();
  liv_loop();

}
void door_loop()
{
  char key = 0;
  lcd.clear();
  delay(100);
  lcd.setCursor(0, 1);
  lcd.print("* for RFID");
  Serial.println("* for RFID");
  lcd.setCursor(1, 1);
  lcd.print("# for pass");
  Serial.println("# for pass");

  key = kpd.getKey();
  if ('*' == key)
  {
    enter_pass();
  }
  else if ('#' == key)
  {
    scan_card();
  }
}
void kit_loop()
{
  int gas = analogRead(GAS_SS_PIN);
  int chk = DHT_KIT.read11(DHT11_SS_2_PIN);
  int temp = DHT_KIT.temperature;
  int light = analogRead(LIGHT_SS_PIN);

  Serial.print("Gas radian: ");
  Serial.println(gas);
  Serial.print("Temp in kit: ");
  Serial.println(temp);
  Serial.print("Light in kit: ");
  Serial.println(light);

  if (gas >= 800)
  {
    while (gas >= 800)
    {
      Serial.println("GAS WARNING");
      gas_warning();
      turn_on_kit_fan();
      turn_off_kit_ligh();
      gas = analogRead(GAS_SS_PIN);
      goto GAS_WARNING;
    }
  }

  if (30 <= temp)
  {
    Serial.println("temp hig");
    turn_on_kit_fan();
  }
  else if ((30 > temp) && (temp > 0))
  {
    Serial.println("temp low");
    turn_off_kit_fan();
  }

  if (light > 900)
  {
    Serial.println("morning");
    turn_off_kit_ligh();
  }
  else if ((light < 50) && (light > 0))
  {
    Serial.println("night");
    turn_on_kit_ligh();

  }

GAS_WARNING:
  delay(500);
}

void liv_loop()
{
  int chk = DHT_LIV.read11(DHT11_SS_1_PIN);
  int temp = DHT_LIV.temperature;
  int light = analogRead(LIGHT_SS_PIN);

  Serial.print("Temp in Liv: ");
  Serial.println(temp);
  Serial.print("Light in Liv: ");
  Serial.println(light);

  if (30 <= temp)
  {
    Serial.println("temp hig liv");
    turn_on_liv_fan();
  }
  else if ((30 > temp) && (temp > 0))
  {
    Serial.println("temp low liv");
    turn_off_liv_fan();
  }

  if ((light < 50) && (light > 0))
  {
    Serial.println("night");
    turn_off_liv_light();
  }
  else if (light > 900)
  {
    Serial.println("morning");
    turn_on_liv_light();
  }
  delay(500);
}

void scan_card()
{
  char card[4];
  int count_time;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CARD PLEASE");
  delay(100);
  count_time = 0;
  lcd.setCursor(0, 1);
  while (1)
  {
    // Select one of the cards
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial())
    {
      delay(100);
      break;
    }

    // Dump debug info about the card; PICC_HaltA() is automatically called
    if (count_time == 100) break;
    count_time++;
    delay(100);
  }
  if (count_time < 100)
  {
    int i;
    for (i = 0; i < 4; i++)
    {
      card[i] = mfrc522.uid.uidByte[i];
    }
    Serial.println("OK");
    if (memcmp(card, card_id, 4) == 0)
    {

      lcd.clear();
      delay(100);
      lcd.setCursor(0, 0);
      lcd.print("OPEN DOOR");
      delay(1000);
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
      lcd.setCursor(0, 0);
      lcd.print("ACCESS DENIE");
      delay(1000);
      //      door_warning();
    }
  }
  else
  {
    lcd.clear();
    delay(100);
    lcd.setCursor(0, 0);
    lcd.print("TIMEOUT");
    delay(1000);
  }
}

void enter_pass()
{
  char pass_enter[4];
  char key = 0;
  int count = 0;
  int i = 0;
  lcd.clear();
  delay(100);
  lcd.setCursor(0, 0);
  lcd.print("ENTER PASS");
  Serial.println("ENTER PASS");
  delay(100);
  lcd.setCursor(0, 1);
  while (i < 300)
  {
    key = kpd.getKey();
    if (key)
    {
      pass_enter[count] = key;
      i = 0;
      count++;
      lcd.setCursor(count, 1);
      lcd.print("*");
    }
    if (count == 4)
    {
      break;
    }
    i++;
    delay(10);
  }

  if (count < 4)
  {
    lcd.clear();
    delay(100);
    lcd.setCursor(0, 0);
    lcd.print("TIMEOUT");
    delay(1000);
  }
  else
  {
    if (!memcmp(pass, pass_enter, 4))
    {

      lcd.clear();
      delay(100);
      lcd.setCursor(0, 0);
      lcd.print("OPEN DOOR");
      delay(1000);
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
      lcd.setCursor(0, 0);
      lcd.print("Wrong Password");
      delay(1000);
      //      door_warning();
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
  int i;

  for (i = 0; i < 90; i++)
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

  for (i = 0; i < 90; i++)
  {
    servo_windown.write(pos);
    pos--;
    delay(50);
  }
}

void gas_warning()
{
  int i;

  for (i = 0; i < 20; i++)
  {
    digitalWrite(BUZZER_GAS, LOW);
    delay(100);
    digitalWrite(BUZZER_GAS, HIGH);
    delay(50);
  }
}

void door_warning()
{
  int i;

  for (i = 0; i < 10; i++)
  {
    digitalWrite(BUZZER_GAS, LOW);
    delay(500);
    digitalWrite(BUZZER_GAS, HIGH);
    delay(100);
  }
}
