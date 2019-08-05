#include <AFMotor.h>
#include <SoftwareSerial.h>
#include <HCSR04.h>

#define SENSOR_L      A2
#define SENSOR_F      A3
#define SENSOR_R      A4

SoftwareSerial BLSerial(A0, A1);//A0 is RX pin, A1 is TX pin
// DC motor on M2
AF_DCMotor mleft(1);
AF_DCMotor mrigh(2);
int control_comm;
int auto_start;
void setup() {
  control_comm = 1;
  auto_start = 1;
  //  pinMode(SENSOR_L, INPUT);
  //  pinMode(SENSOR_F, INPUT);
  //  pinMode(SENSOR_R, INPUT);
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("HELLO!");
  BLSerial.begin(9600);

  BLSerial.println("Hello Connect");
  // turn on motor #2
  mleft.setSpeed(200);
  mleft.run(RELEASE);
  mrigh.setSpeed(200);
  mrigh.run(RELEASE);
}

int zikzak(int first)
{
  int i;
  unsigned long time_count;
  int dodge_ob = 0;
  int bl_data = 'A';

  BLSerial.println("In Automation");
  Serial.println("In Automation");
  i = 0;
  while (i < 90)
  {
    turn_left();
    delayMicroseconds(20);
    i++;
    Serial.print("in L ");
    dodge_ob = detect_ob(1);
    if(dodge_ob == 1)
    {
      break;
    }
  }

  mleft.run(RELEASE);
  mrigh.run(RELEASE);
  delayMicroseconds(50);
  if (BLSerial.available())
  {
    Serial.println("In DATA");
    bl_data = BLSerial.read();
  }
  if ('M' == bl_data)
  {
    return 1;
  }
  else if ('E' == bl_data)
  {
    return 2;
  }
  i = 0;
  while (i < 100)
  {
    i++;
    Serial.println("in F1 ");
    forward();
    delayMicroseconds(20);
    dodge_ob = detect_ob(1);
    if(dodge_ob == 1)
    {
      break;
    }

  }

  mleft.run(RELEASE);
  mrigh.run(RELEASE);
  delayMicroseconds(50);
  if (BLSerial.available())
  {
    Serial.println("In DATA");
    bl_data = BLSerial.read();
  }
  if ('M' == bl_data)
  {
    return 1;
  }
  else if ('E' == bl_data)
  {
    return 2;
  }
  i = 0;
  while (i < 90)
  {
    i++;
    Serial.println("in R ");
    turn_right();
    delayMicroseconds(20);
    dodge_ob = detect_ob(1);
    if(dodge_ob == 1)
    {
      break;
    }
  }

  mleft.run(RELEASE);
  mrigh.run(RELEASE);
  delayMicroseconds(50);
  if (BLSerial.available())
  {
    Serial.println("In DATA");
    bl_data = BLSerial.read();
  }
  if ('M' == bl_data)
  {
    return 1;
  }
  else if ('E' == bl_data)
  {
    return 2;
  }

  mleft.run(RELEASE);
  mrigh.run(RELEASE);
  delayMicroseconds(50);
  i = 0;
  while (i < 100)
  {
    i++;
    Serial.println("in F2 ");
    forward();
    delayMicroseconds(20);
    dodge_ob = detect_ob(1);
    if(dodge_ob == 1)
    {
      break;
    }

  }

  mleft.run(RELEASE);
  mrigh.run(RELEASE);
  delayMicroseconds(50);
  if (BLSerial.available())
  {
    Serial.println("In DATA");
    bl_data = BLSerial.read();
  }
  if ('M' == bl_data)
  {
    return 1;
  }
  else if ('E' == bl_data)
  {
    return 2;
  }
  return 0;

}

int manual_con()
{
  int bl_comm;
  int i = 0;
  bl_comm = 'Y';
  mleft.setSpeed(200);
  mleft.run(RELEASE);
  mrigh.setSpeed(200);
  mrigh.run(RELEASE);
  Serial.println("In Manual");
  if (BLSerial.available())
  {
    bl_comm = BLSerial.read();
    delay(10);
  }
  if (bl_comm != 'Y')
  {
    Serial.println(bl_comm);
  }
  if ('L' == bl_comm)
  {
    mleft.run(RELEASE);
    mrigh.run(FORWARD);
    while (i < 30)
    {
      mleft.setSpeed(60);
      mrigh.setSpeed(60);
      i++;
      delay(10);
    }
  }
  else if ('R' == bl_comm)
  {
    mleft.run(FORWARD);
    mrigh.run(RELEASE);
    while (i < 30)
    {
      mleft.setSpeed(60);
      mrigh.setSpeed(60);
      i++;
      delay(10);
    }
  }
  else if ('F' == bl_comm)
  {
    mleft.run(FORWARD);
    mrigh.run(FORWARD);
    while (i < 30)
    {
      mleft.setSpeed(60);
      mrigh.setSpeed(60);
      i++;
      delay(10);
    }
  }
  else if ('B' == bl_comm)
  {
    mleft.run(BACKWARD);
    mrigh.run(BACKWARD);
    while (i < 30)
    {
      mleft.setSpeed(60);
      mrigh.setSpeed(60);
      i++;
      delay(10);
    }
  }
  else if ('A' == bl_comm)
  {
    return 0;
  }
  else if ('E' == bl_comm)
  {
    return 2;
  }
  return 1;
}

int Estop()
{
  int bl_data;

  if (BLSerial.available())
  {
    Serial.println("In DATA");
    bl_data = BLSerial.read();
    if ('A' == bl_data)
    {
      return 0;
    }
    else if ('M' == bl_data)
    {
      return 1;
    }
    else
    {

    }
  }
  Serial.println("In Stop");
  mleft.run(RELEASE);
  mrigh.run(RELEASE);
  delay(100);
  return 2;
}
// Test the DC motor, stepper and servo ALL AT ONCE!
void loop()
{

  if (control_comm == 0)
  {
    control_comm = zikzak(auto_start);
    auto_start = 0;
  }
  else if (control_comm == 1)
  {
    control_comm = manual_con();
    auto_start = 1;
  }
  else
  {
    control_comm = Estop();
    auto_start = 1;
  }
}

void turn_left()
{
  mleft.run(RELEASE);
  mrigh.run(FORWARD);
  mleft.setSpeed(45);
  mrigh.setSpeed(45);
  //  delayMicroseconds(10);
  //  mleft.setSpeed(40);
  //  mrigh.setSpeed(40);
}

void turn_right()
{
  mleft.run(FORWARD);
  mrigh.run(RELEASE);
  mleft.setSpeed(45);
  mrigh.setSpeed(45);
  //  delayMicroseconds(10);
  //  mleft.setSpeed(40);
  //  mrigh.setSpeed(40);
}

void forward()
{
  mleft.run(FORWARD);
  mrigh.run(FORWARD);
  mleft.setSpeed(45);
  mrigh.setSpeed(45);
  //  delayMicroseconds(10);
  //  mleft.setSpeed(40);
  //  mrigh.setSpeed(40);
}

void backward()
{
  mleft.run(BACKWARD);
  mrigh.run(BACKWARD);
  mleft.setSpeed(45);
  mrigh.setSpeed(45);
  //  delayMicroseconds(10);
  //  mleft.setSpeed(40);
  //  mrigh.setSpeed(40);
}

int detect_ob(int dir)
{
  int break_cycle;

  break_cycle = 0;
  if (50 >= analogRead(SENSOR_F))
  {
      mleft.run(RELEASE);
  mrigh.run(RELEASE);
    delayMicroseconds(10);
    while (50 >= analogRead(SENSOR_F))
    {
      break_cycle = 1;
      Serial.println("Object Front");
      backward();
      delay(900);
    }
  }

  if (50 >= analogRead(SENSOR_L))
  {
      mleft.run(RELEASE);
  mrigh.run(RELEASE);
    delayMicroseconds(10);
    while (50 >= analogRead(SENSOR_L))
    {
      break_cycle = 1;
      Serial.println("Object LEFT");
      turn_right();
      delayMicroseconds(626);
    }
  }

  if (50 >= analogRead(SENSOR_R))
  {
      mleft.run(RELEASE);
  mrigh.run(RELEASE);
    delayMicroseconds(10);
    while (50 >= analogRead(SENSOR_R))
    {
      break_cycle = 1;
      Serial.println("Object Right");
      turn_left();
      delayMicroseconds(626);
    }
  }
  return break_cycle;
}
