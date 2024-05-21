//libray here
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <DHT_U.h>

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
unsigned int TEMPERATURE = 0; //setting val(not real)
unsigned int HUMIDITY = 0; //setting val(not real)

#define MENU_BTN_PIN  3
#define UP_BTN_PIN 6
#define DOWN_BTN_PIN 7
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
int Relaypin1 = 4;
int Relaypin2 = 5;

/*system control : 자신이 사용하는 부분_ms*/
/*reference : unsigned int SWITCH_MS = 0;*/

/*contorl pannel*/
unsigned int CONTROL_PANNEL_MS = 0;
bool IS_LCD_UPDATE = false;

/*온도 습도 측정 후 제어*/
unsigned int controlValue_MS = 0;

void setup()
{
  controlPannelBegin();
  relayBegin();
  dht.begin();
}

void loop()
{
  ControlPannel();
  ControlValue();
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
        //temperature
        TEMPERATURE = TEMPERATURE++%50; //max 49
        IS_LCD_UPDATED = false; 
      }
      else if(MODE == 4)
      {
        //humidity
        HUMIDITY = HUMIDITY++%100; //max 49
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
        //temperature
        TEMPERATURE--; //max 49
        if(TEMPERATURE < 0) TEMPERATURE = 0;
        IS_LCD_UPDATED = false; 
      }
      else if(MODE == 4)
      {
        //humidity
        HUMIDITY--; //max 49
        if(HUMIDITY < 0) HUMIDITY = 0;
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
      printTemperature();
    else if(MODE == 4)
      printHumidity();

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

void printTemperature()
{
  lcd.clear();
  lcd.print("TEMP:")
  lcd.print(TEMPERATURE);
  lcd.print("C");
}

void printHumidity()
{
  lcd.clear();
  lcd.print("HUMI:")
  lcd.print(HUMIDITY);
  lcd.print("%");
}

/*추가한 부분*/
void ControlValue(){
  if(controlValue_MS > 100) {
  getValue();
  onOffh(HUMIDITY);
  onOfft(TEMPERATURE);

  controlValue_MS = 0;
  }
  else
    controlValue_MS++;
}

void relayBegin() {
  Serial.begin(9600);
  pinMode(Relaypin1, OUTPUT);
  pinMode(Relaypin2, OUTPUT);
}

void getValue(){
 TEMPERATURE = dht.readTemperature();
 HUMIDITY = dht.readHumidity();
}

void onOffh(unsigned int h) {
 if (h < 50 ) {
   digitalWrite(Relaypin1, HIGH); //가습기 ON 
 }else { 
   digitalWrite(Relaypin1,LOW); 
 }
}

void onOfft(unsigned int t) {
 if (t < 15) {
   digitalWrite(Relaypin2, HIGH); //온열패드 ON 
 }else { 
   digitalWrite(Relaypin2,LOW); 
 }
}
