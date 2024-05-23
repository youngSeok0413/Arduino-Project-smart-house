//libray here
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Stepper.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);
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

#define MENU_BTN_PIN  3
#define UP_BTN_PIN 6
#define DOWN_BTN_PIN 7

/*contorl pannel*/
unsigned int CONTROL_PANNEL_MS = 0;
bool IS_LCD_UPDATE = false;

/*feedback system(relay module)*/
#define RELAYPIN1 4;
#define RELAYPIN2 5;
#define DHTPIN 2
#define DHTTYPE DHT11
/*온도 습도 측정 후 제어*/
unsigned int FEED_SYSTEM_MS = 0;

/*feedback system(motor module)*/
#define STEPPER_PIN_1 8
#define STEPPER_PIN_2 9
#define STEPPER_PIN_3 10
#define STEPPER_PIN_4 11

#define STEPPERREVOL 1024
#define STEPVEL 30

#define FULLY_OPENED_NUM 100   

unsigned int WINDOW_ROTATED_NUM
bool IS_WINDOW_OPEN = false;

void setup()
{
  ControlPannelInit();
  FeedSystemInit();
}

void loop()
{
  ControlPannel();
  FeedSystem();

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
  if(SWITCH_MS > 10)
  {
    if(digitalRead(MENU_BTN_PIN) == LOW)
    {
      MODE = MODE++%5;

      IS_LCD_UPDATED = false;
      CONTROL_PANNEL_MS = 0;
    }
    else if(digitalRead(UP_BTN_PIN) == LOW)
    {
      if(MODE == 1)
      {
        //hour
        HOUR = HOUR++%24;
        IS_LCD_UPDATED = false;
      }
      else if(MODE == 2)
      {
        //minute
        MINUTE = MINUTE++%60;
        IS_LCD_UPDATED = false;
      }
      else if(MODE == 3)
      {
        //SETtING_Temperature
        SETTING_TEMPERATURE = SETTING_TEMPERATURE++%50; //max 49
        IS_LCD_UPDATED = false; 
      }
      else if(MODE == 4)
      {
        //SETTING_humidity
        SETTING_HUMIDITY = SETTING_HUMIDITY++%100; //max 49
        IS_LCD_UPDATED = false;
      }

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
      
      CONTROL_PANNEL_MS = 0;
    }
    else
      CONTROL_PANNEL_MS++;
  }  
}

void lcdUpdate()
{
  if(!IS_LCD_UPDATE)
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

    IS_LCD_UPDATE = true;
  }
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
    lcd.println(MINUTE);
  }
  else
    lcd.println(MINUTE);
  
  lcd.print("TEMP"); //real data
  lcd.print("C, ");
  lcd.print("HUM"); //real data
  lcd.print("%");
}

void printSETTING_Temperature()
{
  lcd.clear();
  lcd.print("TEMP:")
  lcd.print(SETTING_TEMPERATURE);
  lcd.print("C");
}

void printSETTING_Humidity()
{
  lcd.clear();
  lcd.print("HUMI:")
  lcd.print(SETTING_HUMIDITY);
  lcd.print("%");
}

/*추가한 부분*/
void FeedSystem(){
  if(FEED_SYSTEM_MS > 100) {
    
    int temp = getTemperature();
    int humi = getHumidity();

    onOfft(temp);
    onOffh(humi);
    controlWindow(temp, humi);

    FEED_SYSTEM_MS = 0;
  }
  else
    FEED_SYSTEM_MS++;
}

/*feedback system*/
void FeedSystemInit() {
  dht.begin();
  pinMode(RELAYPIN1, OUTPUT);
  pinMode(RELAYPIN2, OUTPUT);
  stepper.setSpeed(STEPVEL); 
}

//getvalue (온도 습도)
int getTemperature()
{
  return dht.readSETTING_Temperature();
}

int getHumidity()
{
  return dht.readSETTING_Humidity();
}

void onOffh(unsigned int h) {
 if (h < SETTING_HUMIDITY ) {
   digitalWrite(RELAYPIN1, HIGH); //가습기 ON 
 }else { 
   digitalWrite(RELAYPIN1, LOW); 
 }
}

void onOfft(unsigned int t) {
 if (t < SETTING_TEMPERATURE) {
   digitalWrite(RELAYPIN2, HIGH); //온열패드 ON 
 }else { 
   digitalWrite(RELAYPIN2,LOW); 
 }
}

void controlWindow(unsigned int t, unsigned int h)
{
  if(t > SETTING_TEMPERATURE || h > SETTING_HUMIDITY)
  {
    if(!IS_WINDOW_OPEN)
    {
      stepper.step(FULLY_OPENED_NUM);
      IS_WINDOW_OPEN = true;
    }
  }
  else
  {
    if(IS_WINDOW_OPEN)
    {
      stepper.step(-1*FULLY_OPENED_NUM);
      IS_WINDOW_OPEN = false;
    }
  }
}

/*feedback system : motor*/

