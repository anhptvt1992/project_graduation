#include <AFMotor.h>
#include <SoftwareSerial.h>
#include <HCSR04.h>

UltraSonicDistanceSensor distanceSensor(A3, A2);//A3 is trigger pin, A2 is echo pin
SoftwareSerial BLSerial(A0, A1);//A0 is RX pin, A1 is TX pin
// DC motor on M2
AF_DCMotor mleft(2);
AF_DCMotor mrigh(1);
int control_comm;
int auto_start;
void setup() {
  control_comm = 0;
  auto_start = 1;
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Motor party!");
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
  int dis_ob = 9;
  int bl_data = 'A';
  
  BLSerial.println("In Automation");
  Serial.println("In Automation");
  
  mleft.setSpeed(200);
  mleft.run(RELEASE);
  mrigh.setSpeed(200);
  mrigh.run(RELEASE);
  delayMicroseconds(500);
  turn_left();
  i = 0;
  while (i < 70)
  {
    dis_ob = distanceSensor.measureDistanceCm();
    if ((8 >= dis_ob) && (0 <= dis_ob))
    {
      turn_right();
      i = 45;
    }
    i++;
    delayMicroseconds(10);
  }

  mleft.setSpeed(200);
  mleft.run(RELEASE);
  mrigh.setSpeed(200);
  mrigh.run(RELEASE);
  delayMicroseconds(500);
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
  
  forward();
  i = 0;
  dodge_ob = 0;
  while (i < 130)
  {
    dis_ob = distanceSensor.measureDistanceCm();
    if ((8 >= dis_ob) && (0 <= dis_ob))
    {
      turn_right();
      i = 90;
    }
    i++;
    delayMicroseconds(10);
  }

  mleft.setSpeed(200);
  mleft.run(RELEASE);
  mrigh.setSpeed(200);
  mrigh.run(RELEASE);
  delayMicroseconds(500);
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
  
  turn_right();
  i = 0;
  dodge_ob = 0;
  while (i < 70)
  {
    dis_ob = distanceSensor.measureDistanceCm();
    if ((8 >= dis_ob) && (0 <= dis_ob))
    {
      turn_left();
      i = 45;
    }
    i++;
    delayMicroseconds(10);
  }

  mleft.setSpeed(200);
  mleft.run(RELEASE);
  mrigh.setSpeed(200);
  mrigh.run(RELEASE);
  delayMicroseconds(500);
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
  
  forward();
  i = 0;
  dodge_ob = 0;
  while (i < 130)
  {
    dis_ob = distanceSensor.measureDistanceCm();
    if ((8 >= dis_ob) && (0 <= dis_ob))
    {
      turn_left();
      i = 90;
    }
    i++;
    delayMicroseconds(10);
  }
  
  mleft.setSpeed(200);
  mleft.run(RELEASE);
  mrigh.setSpeed(200);
  mrigh.run(RELEASE);
  delayMicroseconds(500);
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
  mleft.setSpeed(200);
  mleft.run(RELEASE);
  mrigh.setSpeed(200);
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
  mleft.setSpeed(255);
  mrigh.setSpeed(255);
  delayMicroseconds(10);
  mleft.setSpeed(40);
  mrigh.setSpeed(40);
}

void turn_right()
{
  mleft.run(FORWARD);
  mrigh.run(RELEASE);
  mleft.setSpeed(255);
  mrigh.setSpeed(255);
  delayMicroseconds(10);
  mleft.setSpeed(40);
  mrigh.setSpeed(40);
}

void forward()
{
  mleft.run(FORWARD);
  mrigh.run(FORWARD);
  mleft.setSpeed(255);
  mrigh.setSpeed(255);
  delayMicroseconds(10);
  mleft.setSpeed(40);
  mrigh.setSpeed(40);
}

void backward()
{
  mleft.run(BACKWARD);
  mrigh.run(BACKWARD);
  mleft.setSpeed(255);
  mrigh.setSpeed(255);
  delayMicroseconds(10);
  mleft.setSpeed(40);
  mrigh.setSpeed(40);
}
