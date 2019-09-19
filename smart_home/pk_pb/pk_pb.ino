#include <dht.h>
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

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

#define RST_PIN           49        // Using for RCC-522
#define SS_PIN            53        // Using for RCC-522

/*Kitchen*/
//#define DHT11_PB_2_PIN    8     // USING for DHT kitchen
#define WD_RC_SERVO       2     // Using Control RC Servo windown at kitchen
#define GAS_SS_PIN        A0      // USING for ADC  GAS SENSOR MQ-02
#define FAN_KIT_PIN       9     // Control relay fan in kitchen
#define LIG_KIT_PIN       8      // Control relay light in kitchen 

/*Bedroom*/
#define WD_BED_SERVO      3    // Using Control RC Servo windown at bedroom
#define FAN_BED_PIN       A2     // Control relay fan in bedroom
#define LIG_BED_PIN       A3      // Control relay light in bedroom  

/*Livingroom*/
//#define DHT11_PK_1_PIN    7     // Using for DHT Livingroom
#define LIGHT_SS_PIN      A1     // Using like input read  
#define FAN_LIV_PIN       13      // Control relay fan in Livingroom
#define LIG_LIV_PIN       12      // Control relay light in Livingroom

/*Outside and another define*/
#define LIG_OUTSIDE_PIN   10      // Control relay light outside
#define OPEN_DOOR         5     // Using to open main door
#define CLOSE_DOOR        6     // Using to close main door
#define BUZZER_GAS        7     // Using buzz waring for gas leakage  
#define NO_OF_ROW         4     // Using for keypad 
#define NO_OF_COL         3     // Using for keypad 

#define ESP_TX            A5    //TX MEGA - RX ESP
#define ESP_RX            A6    //RX MEGA - TX ESP

#define KIT_WIN_CTRL      22    //D0
#define KIT_LIG_CTRL      23    //D1
#define KIT_FAN_CTRL      24    //D2
#define LIV_LIG_CTRL      25    //D3
#define LIV_FAN_CTRL      26    //D4
#define BED_WIN_CTRL      27    //D5
#define BED_LIG_CTRL      28    //D6
#define BED_FAN_CTRL      29    //D7

void door_loop();
void kit_loop();
void liv_loop();
MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo servo_windown;//use for kitchen
Servo servo_bed_wd; //use for bedroom
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

int wd_kit_status;//0 is nothing, 1 is close, 2 is open
int wd_bed_status;//0 is nothing, 1 is close, 2 is open

void setup()
{
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();    // Init MFRC522
  Serial.begin(9600);
  pinMode(FAN_KIT_PIN, OUTPUT);
  pinMode(FAN_LIV_PIN, OUTPUT);
  pinMode(FAN_BED_PIN, OUTPUT);
  pinMode(LIG_KIT_PIN, OUTPUT);
  pinMode(LIG_LIV_PIN, OUTPUT);
  pinMode(LIG_BED_PIN, OUTPUT);
  pinMode(LIG_OUTSIDE_PIN, OUTPUT);
  pinMode(OPEN_DOOR, OUTPUT);
  pinMode(CLOSE_DOOR, OUTPUT);
  pinMode(BUZZER_GAS, OUTPUT);
  pinMode(KIT_WIN_CTRL, INPUT);
  pinMode(KIT_LIG_CTRL, INPUT);
  pinMode(KIT_FAN_CTRL, INPUT);
  pinMode(LIV_LIG_CTRL, INPUT);
  pinMode(LIV_FAN_CTRL, INPUT);
  pinMode(BED_WIN_CTRL, INPUT);
  pinMode(BED_LIG_CTRL, INPUT);
  pinMode(BED_FAN_CTRL, INPUT);
  //  pinMode(LIGHT_SS_PIN, INPUT);
  digitalWrite(FAN_KIT_PIN, HIGH);
  digitalWrite(LIG_KIT_PIN, HIGH);
  digitalWrite(FAN_LIV_PIN, HIGH);
  digitalWrite(LIG_LIV_PIN, HIGH);
  digitalWrite(OPEN_DOOR, LOW);
  digitalWrite(CLOSE_DOOR, LOW);
  digitalWrite(BUZZER_GAS, HIGH);

  servo_windown.attach(WD_RC_SERVO);
  servo_bed_wd.attach(WD_BED_SERVO);

  //  Serial.println("Hello");
  lcd.init();  // initialize the lcd
  //  Serial.println("Finish LCD INIT");
  // Print a message to the LCD.
  lcd.backlight();
  lcd.print("Hello");
  wd_kit_status = 0;
  wd_bed_status = 0;
  close_wd_kit();
  close_wd_bed();
}

/*this loop will use for close or open door*/
void loop()
{
  door_loop();
  outside_light_control();
  scan_esp_com();
  gas_detect();

}
void door_loop()
{
  char key = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("# for RFID");
  lcd.setCursor(0, 1);
  lcd.print("* for pass");

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
void outside_light_control()
{
  int light = analogRead(LIGHT_SS_PIN);

  if (light > 900)
  {
    Serial.println("night");
    turn_on_outside_light();
  }
  else if ((light < 200) && (light > 0))
  {

    Serial.println("morning");
    turn_off_outside_light();
  }
}

void gas_detect()
{
  int gas = analogRead(GAS_SS_PIN);
  if (gas >= 200)
  {
    Serial.print("GAS Warning ");
    Serial.println(gas);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("GAS WARNING");
    gas_warning();
    delay(100);
    open_windown();
    open_door();
    delay(500);
    stop_door();
    while (gas >= 200)
    {
      gas_warning();
      gas = analogRead(GAS_SS_PIN);
    }
    close_windown();
    close_door();
    delay(500);
    stop_door();
  }
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
    if (memcmp(card, card_id, 4) == 0)
    {
      lcd.clear();
      delay(100);
      lcd.setCursor(0, 0);
      lcd.print("OPEN DOOR");
      delay(1000);
      open_door();
      delay(500);
      stop_door();
      delay(3000);
      close_door();
      delay(500);
      stop_door();
    }
    else
    {
      lcd.clear();
      delay(100);
      lcd.setCursor(0, 0);
      lcd.print("ACCESS DENIE");
      delay(500);
    }
  }
  else
  {
    lcd.clear();
    delay(100);
    lcd.setCursor(0, 0);
    lcd.print("TIMEOUT");
    delay(500);
  }
}

void enter_pass()
{
  char pass_enter[4];
  char key = 0;
  int count = 0;
  int i = 0;
  lcd.clear();
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
    delay(500);
  }
  else
  {
    if (!memcmp(pass, pass_enter, 4))
    {

      lcd.clear();
      delay(100);
      lcd.setCursor(0, 0);
      lcd.print("OPEN DOOR");
      delayMicroseconds(1000);
      open_door();
      delay(500);
      stop_door();
      delay(3000);
      close_door();
      delay(500);
      stop_door();
    }
    else
    {
      lcd.clear();
      delay(100);
      lcd.setCursor(0, 0);
      lcd.print("Wrong Password");
      delay(600);
    }
  }
}

void turn_on_liv_fan()
{
  digitalWrite(FAN_LIV_PIN, LOW);
}

void turn_off_liv_fan()
{
  digitalWrite(FAN_LIV_PIN, HIGH);
}

void turn_on_kit_fan()
{
  digitalWrite(FAN_KIT_PIN, LOW);
}

void turn_off_kit_fan()
{
  digitalWrite(FAN_KIT_PIN, HIGH);
}

void turn_on_bed_fan()
{
  digitalWrite(FAN_BED_PIN, LOW);
}

void turn_off_bed_fan()
{
  digitalWrite(FAN_BED_PIN, HIGH);
}

void turn_on_liv_light()
{
  digitalWrite(LIG_LIV_PIN, LOW);
}

void turn_off_liv_light()
{
  digitalWrite(LIG_LIV_PIN, HIGH);
}

void turn_on_kit_ligh()
{
  digitalWrite(LIG_KIT_PIN, LOW);
}

void turn_off_kit_ligh()
{
  digitalWrite(LIG_KIT_PIN, HIGH);
}

void turn_on_bed_light()
{
  digitalWrite(LIG_BED_PIN, LOW);
}

void turn_off_bed_light()
{
  digitalWrite(LIG_BED_PIN, HIGH);
}

void turn_on_outside_light()
{
  digitalWrite(LIG_OUTSIDE_PIN, HIGH);
}

void turn_off_outside_light()
{
  digitalWrite(LIG_OUTSIDE_PIN, LOW);
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

  for (i = 0; i < 100; i++)
  {
    servo_windown.write(pos);
    servo_bed_wd.write(pos);
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
    servo_bed_wd.write(pos);
    pos--;
    delay(50);
  }
}

void open_wd_kit()
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

void close_wd_kit()
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

void open_wb_bed()
{
  int pos = 0;
  int i;

  for (i = 0; i < 100; i++)
  {
    servo_bed_wd.write(pos);
    pos++;
    delay(50);
  }
}

void close_wd_bed()
{
  int pos = 100;
  int i;

  for (i = 0; i < 100 ; i++)
  {
    servo_bed_wd.write(pos);
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
    delayMicroseconds(500);
    digitalWrite(BUZZER_GAS, HIGH);
    delayMicroseconds(100);
  }
}

void scan_esp_com()
{
  if (digitalRead(LIV_FAN_CTRL))
  {
    turn_on_liv_fan();
  }
  else
  {
    turn_off_liv_fan();
  }

  if (digitalRead(KIT_FAN_CTRL))
  {
    turn_on_kit_fan();
  }
  else
  {
    turn_off_kit_fan();
  }

  if (digitalRead(BED_FAN_CTRL))
  {
    turn_on_bed_fan();
  }
  else
  {
    turn_off_bed_fan();
  }

  if (digitalRead(LIV_LIG_CTRL))
  {
    turn_on_liv_light();
  }
  else
  {
    turn_off_liv_light();
  }

  if (digitalRead(KIT_LIG_CTRL))
  {
    turn_on_kit_ligh();
  }
  else
  {
    turn_off_kit_ligh();
  }

  if (digitalRead(BED_LIG_CTRL))
  {
    turn_on_bed_light();
  }
  else
  {
    turn_off_bed_light();
  }

  if (digitalRead(KIT_WIN_CTRL))
  {
    if(wd_kit_status == 0)
    {
      open_wd_kit();
      wd_kit_status = 1;
    }
    
  }
  else
  {
    if(wd_kit_status == 1)
    {
      close_wd_kit();
      wd_kit_status = 0;
    }
    
  }

  if (digitalRead(BED_WIN_CTRL))
  {
    Serial.println("WD BED OPEN");
    if(wd_bed_status == 0)
    {
      open_wb_bed();
      wd_bed_status = 1;
    }
    
  }
  else
  {
    Serial.println("WD BED CLOSE");
    if(wd_bed_status == 1)
    {
      close_wd_bed();
      wd_bed_status = 0;
    }
    
  }
}
