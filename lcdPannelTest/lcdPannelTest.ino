//libray here
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>

#define MENU_BTN_PIN  3
#define UP_BTN_PIN 6
#define DOWN_BTN_PIN 7
#define STEPPER_PIN_1 8
#define STEPPER_PIN_2 9
#define STEPPER_PIN_3 10
#define STEPPER_PIN_4 11
#define STEPPERREVOL 1024
#define STEPVEL 30
#define FULLY_OPENED_NUM 100   

LiquidCrystal_I2C lcd(0x27, 16, 2);
Stepper stepper(STEPPERREVOL, STEPPER_PIN_4, STEPPER_PIN_3, STEPPER_PIN_2, STEPPER_PIN_1);

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

SETtING_Temperature : 
  pin : 2

*/

/*display constant*/

/*MENU MODE : 0(default, clock, SETtING_Temperature, SETTING_humidity), 1 2(set clock : h, m), 3(set SETtING_Temperature), 4(set SETTING_humidity)*/
unsigned int MODE = 0;
/*clock : hour : 0 ~ 23, minute : 0 ~ 59*/
/*SETtING_Temperature(range need to be decided), SETTING_humidity(range need to be decided)*/
unsigned int HOUR = 0;
unsigned int MINUTE = 1;
unsigned int SETTING_TEMPERATURE = 0; //setting val(not real)
unsigned int SETTING_HUMIDITY = 0; //setting val(not real)

/*contorl pannel*/
unsigned int CONTROL_PANNEL_MS = 0;
bool IS_LCD_UPDATED = false;

/*feedback system(relay module)*/
/*온도 습도 측정 후 제어*/
unsigned int FEED_SYSTEM_MS = 0;

/*feedback system(motor module)*/

unsigned int WINDOW_ROTATED_NUM;
bool IS_WINDOW_OPEN = false;

void setup()
{
  ControlPannelInit();
}

void loop()
{
  ControlPannel();

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


/*Contorl pannel*/
void ControlPannelInit()
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

void ControlPannel()
{
  setControlPannel();
  lcdUpdate();
}

void setControlPannel()
{
  if(CONTROL_PANNEL_MS > 10)
  {
    if(digitalRead(MENU_BTN_PIN) == LOW)
    {
      MODE = (MODE+1)%5;

      IS_LCD_UPDATED = false;
      CONTROL_PANNEL_MS = 0;

      delay(200);
    }
    else if(digitalRead(UP_BTN_PIN) == LOW)
    {
      if(MODE == 1)
      {
        //hour
        HOUR = (HOUR+1)%24;
        IS_LCD_UPDATED = false;
      }
      else if(MODE == 2)
      {
        //minute
        MINUTE = (MINUTE+1)%60;
        IS_LCD_UPDATED = false;
      }
      else if(MODE == 3)
      {
        //SETtING_Temperature
        SETTING_TEMPERATURE = (SETTING_TEMPERATURE+1)%50; //max 49
        IS_LCD_UPDATED = false; 
      }
      else if(MODE == 4)
      {
        //SETTING_humidity
        SETTING_HUMIDITY = (SETTING_HUMIDITY+1)%100; //max 49
        IS_LCD_UPDATED = false;
      }

      delay(200);
      CONTROL_PANNEL_MS = 0;
    }
    else if(digitalRead(DOWN_BTN_PIN) == LOW)
    {
      if(MODE == 1)
      {
        //hour
        HOUR--;
        if(HOUR < 0) HOUR = 0;
        IS_LCD_UPDATED = false;
      }
      else if(MODE == 2)
      {
        //minute
        MINUTE--;
        if(MINUTE < 0) MINUTE = 0;
        IS_LCD_UPDATED = false;
      }
      else if(MODE == 3)
      {
        //SETtING_Temperature
        SETTING_TEMPERATURE--; //max 49
        if(SETTING_TEMPERATURE < 0) SETTING_TEMPERATURE = 0;
        IS_LCD_UPDATED = false; 
      }
      else if(MODE == 4)
      {
        //SETTING_humidity
        SETTING_HUMIDITY--; //max 49
        if(SETTING_HUMIDITY < 0) SETTING_HUMIDITY = 0;
        IS_LCD_UPDATED = false;
      }
      
      delay(200);
      CONTROL_PANNEL_MS = 0;
    }
  }
  else
    CONTROL_PANNEL_MS++;  
}

void lcdUpdate()
{
  if(!IS_LCD_UPDATED)
  {
    if(MODE == 0)
      printClock();
    else if(MODE == 1)
      printClock();
    else if(MODE == 2)
      printClock();
    else if(MODE == 3)
      printSETTING_Temperature();
    else if(MODE == 4)
      printSETTING_Humidity();

    IS_LCD_UPDATED = true;
  }
}

void printClock()
{
  lcd.clear();
  lcd.print("TIME:");

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
  
  lcd.setCursor(0, 1);

  lcd.print("TEMP:"); //real data
  lcd.print(10);
  lcd.print("C,HUMI:"); //real data
  lcd.print(10);
  lcd.print("%");
}

void printSETTING_Temperature()
{
  lcd.clear();
  lcd.print("TEMP:");
  lcd.print(SETTING_TEMPERATURE);
  lcd.print("C");
}

void printSETTING_Humidity()
{
  lcd.clear();
  lcd.print("HUMI:");
  lcd.print(SETTING_HUMIDITY);
  lcd.print("%");
}

/*feedback system : motor*/
