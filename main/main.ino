//libray here
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

/*
write here pins that you use(unit : used pin)
arduino uno
lcd : 2 x 16, i2c
  pin : a4, a5

step motor : 
  pin : 8 9 10 11

switch : 
  pin : 3 6 7 (menu, up, down)

relay module : 
  pin : 4, 5

temperature : 
  pin : 2

*/

/*display constant*/

/*MENU MODE : 0(default, clock, temperature, humidity), 1 2(set clock : h, m), 3(set temperature), 4(set humidity)*/
unsigned int MODE = 0;

/*clock : hour : 0 ~ 23, minute : 0 ~ 59*/
/*temperature(range need to be decided), humidity(range need to be decided)*/
unsigned int HOUR = 0;
unsigned int MINUTE = 1;
unsigned int TEMPERATURE = 0;
unsigned int HUMIDITY = 0;

#define MENU_BTN_PIN  3
#define UP_BTN_PIN 6
#define DOWN_BTN_PIN 7

/*system control : 자신이 사용하는 부분_ms*/
unsigned int SWITCH_MS = 0;

void setup()
{
  controlPannelBegin();
}

void loop()
{
  button();

  delay(1);
}

/*control : display, time, variable control*/

/*

reference

void button()
{
  if(SWITCH_MS > 100)
  {
    if(digitalRead(MENU_BTN_PIN) == LOW)
      Serial.println("MENU_BTN");

    SWITCH_MS = 0;
  }
  else
    SWITCH_MS++;
}
*/

void controlPannelBegin()
{
  //serail init
  Serial.begin(9600);

  //lcd init
  lcd.begin();
  lcd.backlight();
  
  //button init
  pinMode(MENU_BTN_PIN, INPUT_PULLUP);
  pinMode(UP_BTN_PIN, INPUT_PULLUP);
  pinMode(DOWN_BTN_PIN, INPUT_PULLUP);
}

void printClock()
{
  lcd.clear();
  lcd.print("TIME : ");

  if(HOUR < 10)
  {
    lcd.print("0");
    lcd.print(HOUR);
  }
  else
    lcd.print(HOUR);
  
  lcd.print(":");
  
  if(MINUTE < 10)
  {
    lcd.print("0");
    lcd.print(MINUTE);
  }
  else
    lcd.print(MINUTE);
}

void setClockHour()
{
  
}

void setClockMinute()
{
  
}

void setTemperature()
{
  
}

void setHumidity()
{
  
}