//libray here
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Stepper.h>

#define MENU_BTN_PIN  3
#define UP_BTN_PIN 6
#define DOWN_BTN_PIN 7
#define RELAYPIN1 4
#define RELAYPIN2 5
#define DHTPIN 2
#define DHTTYPE DHT11
#define STEPPER_PIN_1 8
#define STEPPER_PIN_2 9
#define STEPPER_PIN_3 10
#define STEPPER_PIN_4 11
#define STEPPERREVOL 1024
#define STEPVEL 14
#define FULLY_OPENED_NUM 100   
#define ONE_MINUTE 59999//59999
#define MARGIN 2


LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);
Stepper stepper(2048, STEPPER_PIN_4, STEPPER_PIN_2, STEPPER_PIN_3, STEPPER_PIN_1);

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
unsigned int HOUR = 12;
unsigned int MINUTE = 0;
unsigned int SETTING_TEMPERATURE = 0; //setting val(not real)
unsigned int SETTING_HUMIDITY = 0; //setting val(not real)

/*contorl pannel*/
unsigned int CONTROL_PANNEL_MS = 0;
unsigned int NOW_CLOCK_MS = 0; //60000 1분
bool IS_LCD_UPDATED = false;

/*feedback system(relay module)*/
/*온도 습도 측정 후 제어*/
unsigned int FEED_SYSTEM_MS = 0;

/*feedback system(motor module)*/

int WINDOW_ROTATED_NUM = 2048 * 5;
bool IS_WINDOW_OPEN = false;
bool IS_SETTING = false;

void setup()
{
  ControlPannelInit();
  FeedSystemInit();
}

void loop()
{
  ControlPannel();
  Clock();
  if(!IS_SETTING)
  {
    FeedSystem();
  } 
}

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
      if(MODE != 0) IS_SETTING = true;
      else IS_SETTING = false;
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

void Clock()
{
  if(NOW_CLOCK_MS > ONE_MINUTE)
  {
    MINUTE++;

    if(MINUTE > 59)
    {
      MINUTE = 0;
      HOUR++;
    }
    if(HOUR > 23)
    {
      HOUR = 0;
    }

    IS_LCD_UPDATED = false;
    NOW_CLOCK_MS = 0;
  }
  else
    NOW_CLOCK_MS++;
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

  if(IS_WINDOW_OPEN) lcd.print(" OPEN");
  else lcd.print(" CLOSE");
    
  lcd.setCursor(0, 1);

  lcd.print("TEMP:"); //real data
  lcd.print((unsigned int)getTemperature());
  lcd.print("C,HUM:"); //real data
  lcd.print((unsigned int)getHumidity());
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

  SETTING_HUMIDITY = getHumidity();
  SETTING_TEMPERATURE = getTemperature(); 
}

//getvalue (온도 습도)
int getTemperature()
{
  return dht.readTemperature();
}

int getHumidity()
{
  return dht.readHumidity();
}

void onOffh(unsigned int h) {
 if (h < (SETTING_HUMIDITY - MARGIN)) {
   digitalWrite(RELAYPIN1, HIGH); //가습기 ON
   Serial.println("humid up"); 
 }else { 
   digitalWrite(RELAYPIN1, LOW);
   Serial.println("humid down"); 
 }
}

void onOfft(unsigned int t) {
 if (t < (SETTING_TEMPERATURE - MARGIN)) {
   digitalWrite(RELAYPIN2, HIGH); //온열패드 ON
   Serial.println("temp up"); 
 }else { 
   digitalWrite(RELAYPIN2,LOW);
   Serial.println("temp down"); 
 }
}

void controlWindow(unsigned int t, unsigned int h)
{
  if(t > (SETTING_TEMPERATURE + MARGIN) || h > (SETTING_HUMIDITY +  + MARGIN))
  {
    if(!IS_WINDOW_OPEN){
    
      stepper.step(WINDOW_ROTATED_NUM); //anti clock
      
      Serial.println("open");

      digitalWrite(STEPPER_PIN_1, LOW);
      digitalWrite(STEPPER_PIN_2, LOW);
      digitalWrite(STEPPER_PIN_3, LOW);
      digitalWrite(STEPPER_PIN_4, LOW);

      IS_WINDOW_OPEN = true;
    }
  }
  else
  {
    if(IS_WINDOW_OPEN)
    {
      stepper.step(-1*WINDOW_ROTATED_NUM); //
      
      Serial.println("close");

      digitalWrite(STEPPER_PIN_1, LOW);
      digitalWrite(STEPPER_PIN_2, LOW);
      digitalWrite(STEPPER_PIN_3, LOW);
      digitalWrite(STEPPER_PIN_4, LOW);

      IS_WINDOW_OPEN = false;
    }
  }
}

/*feedback system : motor*/
