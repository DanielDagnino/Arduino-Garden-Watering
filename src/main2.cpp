  //=====================================================================
  //
  //       Garden watering system syncronized with sunrise/sunset
  //       ------------------------------------------------------
  //
  //                      Author: Daniel Dagnino
  //                        My garden, Spain
  //                       Copyright (C) 2017
  //
  // This program is free software; you can redistribute it and/or modify
  // it under the terms of the GNU General Public License as published by
  // the Free Software Foundation; either version 2 of the License, or
  // (at your option) any later version.
  //
  // This program is distributed in the hope that it will be useful,
  // but WITHOUT ANY WARRANTY; without even the implied warranty of
  // MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  // GNU General Public License for more details.
  //
  // You should have received a copy of the GNU General Public License
  // along with this program.  If not, see <http://www.gnu.org/licenses/>.
  //
  //=====================================================================

//----------------------------------------------------------------------//
// Declare libraries:

// Set the led Pin.
#define greenledPin 13
bool greenledState;

// Sunrise/sunset lib.
#include <SRS.h>

// DHT11 lib.
#include <dht.h>

// Wire (for RTC+LCD) lib.
#include <Wire.h>

// RTC DS1307 lib.
#include "RTClib.h"

// LCD lib (F Malpartida's NewLiquidCrystal library).
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

// LCD scaned address: I2C device found at address 0x3F.
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

//----------------------------------------------------------------------//
// Declare variables:

// Set the relay Pin.
#define relayPin 7

// Set the relay for the air pumping.
#define airpump_relay_pin 9
bool pump;
int period_pump_air_min = 3;  // 2 min of air pumping every 24/pump_per_day (pump_per_day times per day).
int pump_per_day = 8;
int time_pump_min;

// Set the DHT Pin.
dht DHT;
#define DHT11_PIN 8

bool new_day = true;
int hum_max_today = 0;
int hum_rain = 80;

// Watering.
unsigned long timeWatering;
unsigned long timeWater_H, timeWater_M;  // Sunrise watering time.
unsigned long timeWater2_H, timeWater2_M;  // Sunset watering time.

// Button variables to define watering time.
const int lcdLight_ButtonPin = 5;
int lcdLight = 0;
bool lcdLight_log = false;

// Button variables to define watering interval.
const int timeIntervalButtonPin = 9;
int ButtonIntervalWateringState;
unsigned long TimeIntervalWatering = 20;   // Time of watering in seconds.
int timeIntervalWatering_M, timeIntervalWatering_S;

// Time variables.
unsigned long timeRTC_H, timeRTC_M;
unsigned long time1, time0 = -10000;

// Init the DS1307.
RTC_DS1307 RTC;
int today_saved;

// 
sun sun;

//----------------------------------------------------------------------//
//----------------------------------------------------------------------//
void setup(){
  // Pins.
  pinMode(greenledPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(airpump_relay_pin, OUTPUT);
  pinMode(lcdLight_ButtonPin, INPUT);
  pinMode(timeIntervalButtonPin, INPUT);
  
  // Serial.
  Serial.begin(9600);
  
  // Wire fot the RTC and LCD.
  Wire.begin();
  
  // Initialize the rtc object
  RTC.begin();
  
//  if (!RTC.isrunning()) {
//    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
//  }
  
  // 
  today_saved = RTC.now().day();
  
  // LCD initialization.
  lcd.begin(16,2);                  
  lcd.backlight();
  lcd.setCursor(0,0); //Start at character 4 on line 0
  lcd.print("Hello, world!");
  lcd.setCursor(0,1);
  lcd.print("1234567890123456");
  lcd.noBacklight();
  
}

//----------------------------------------------------------------------//
//----------------------------------------------------------------------//
// Main loop.
void loop(){
  
  //----------------------------------------------------------------------//
  // Read time from Arduino.
  time1 = millis();
  
  //----------------------------------------------------------------------//
  // Read time from RTC.
  DateTime now = RTC.now();
  
  //----------------------------------------------------------------------//
  // Calculate sunrise/set to water.
  
  // Current day.
  sun.day   = now.day();
  sun.month = now.month();
  sun.year  = now.year();
  
  // Location.
  sun.lat = 41.3851;
  sun.lon = 2.1734;
  sun.timeZone = 2;

  //Time sunrise/set calculation.
  sun.calculate();

  // Define morning and afternoon watering times.
  timeWater_H = sun.sunrise.hour;
  timeWater_M = sun.sunrise.min;
  
  timeWater2_H = sun.sunset.hour;
  timeWater2_M = sun.sunset.min;
  timeWater2_H -= 2;   // Before sunset.

//  // Test mode.
//  timeWater_H = 0;
//  timeWater_M = 0;
//  timeWater2_H = 22;
//  timeWater2_M = 23;
  
  //----------------------------------------------------------------------//
  // Every 10s:
  if ( time1-time0 > 4000 ){
    
//    int batteryVoltageRead = analogRead(A0);
//    float batteryVoltage = float(batteryVoltageRead*(5/1023.));
//    Serial.print("VBat = ");
//    Serial.println(batteryVoltage);
    
    //----------------------------------------------------------------------//
    lcd.clear();
    
    //----------------------------------------------------------------------//
    // Read T and H from DHT.
    int chk = DHT.read11(DHT11_PIN);

    // Max. humidity per day calculation.
    if ( new_day ) hum_max_today = 0;
    else hum_max_today = max(hum_max_today,int(DHT.humidity));

    // Print.
    Serial.print("T = ");
    Serial.print(int(DHT.temperature));   // int because DHT11 does not show fraction.
    Serial.print("C");
    Serial.print("       ");
    Serial.print("H = ");
    Serial.print(int(DHT.humidity));
    Serial.print("%/");
    Serial.print(hum_max_today);
    Serial.println("%");
    
    lcd.setCursor(0,1);
    lcd.write("T=");
    lcd.print(int(DHT.temperature));
    lcd.write("C ");
    lcd.write(" H=");
    lcd.print(int(DHT.humidity));
    lcd.write("%/");
    lcd.print(hum_max_today);
    lcd.write("%");
    
    //----------------------------------------------------------------------//
    // Read time from RTC.
    timeRTC_H = now.hour();
    timeRTC_M = now.minute();

    // Print.
    // Time now.
    Serial.print("TN = ");
    Serial.print(timeRTC_H);
    Serial.print("h");
    Serial.print(timeRTC_M);
    Serial.print("m");
    Serial.print("   ");
    // Schedule Time watering.
    if ( timeRTC_H*60 + timeRTC_M < timeWater_H*60 + timeWater_M ){
      Serial.print("TW = ");
      Serial.print(timeWater_H);
      Serial.print("h");
      Serial.print(timeWater_M);
      Serial.println("m");
      Serial.println();
    }
    else if ( timeRTC_H*60 + timeRTC_M < timeWater2_H*60 + timeWater2_M ){
      Serial.print("TW = ");
      Serial.print(timeWater2_H);
      Serial.print("h");
      Serial.print(timeWater2_M);
      Serial.println("m");
      Serial.println();
    }
    else{
      // Compute the next day watering time.
      sun.day += 1;
      sun.calculate();
      timeWater_H = sun.sunrise.hour;
      timeWater_M = sun.sunrise.min;
      Serial.print("TW = ");
      Serial.print(timeWater_H);
      Serial.print("h");
      Serial.print(timeWater_M);
      Serial.println("m");
      Serial.println();
      sun.day -= 1;
      sun.calculate();
    }
    
    // Time now.
    lcd.setCursor(0,0);
    lcd.print(timeRTC_H);
    lcd.write("h");
    lcd.print(timeRTC_M);
    lcd.write("m");
    lcd.write(" ");
    // Schedule Time watering.
    if ( timeRTC_H*60 + timeRTC_M < timeWater_H*60 + timeWater_M ){
      lcd.write("TW=");
      lcd.print(timeWater_H);
      lcd.write("h");
      lcd.print(timeWater_M);
      lcd.write("m");
    }
    else if ( timeRTC_H*60 + timeRTC_M < timeWater2_H*60 + timeWater2_M ){
      lcd.write("TW=");
      lcd.print(timeWater2_H);
      lcd.write("h");
      lcd.print(timeWater2_M);
      lcd.write("m");
    }
    else{
      // Compute the next day watering time.
      sun.day += 1;
      sun.calculate();
      timeWater_H = sun.sunrise.hour;
      timeWater_M = sun.sunrise.min;
      lcd.write("TW=");
      lcd.print(timeWater_H);
      lcd.write("h");
      lcd.print(timeWater_M);
      lcd.write("m");
      sun.day -= 1;
      sun.calculate();
    }

    // New day?
    if ( now.day() != today_saved ){
      today_saved = now.day();
      new_day = true;
    }
    else new_day = false;
    
    //----------------------------------------------------------------------//
    // Valve state. 
    if ( digitalRead(relayPin)==1 ) digitalWrite(greenledPin, HIGH);
    else digitalWrite(greenledPin, LOW);
    
    //----------------------------------------------------------------------//
    // Update time.
    time0 = time1;
  }
  
  //----------------------------------------------------------------------//
  //----------------------------------------------------------------------//
  //----------------------------------------------------------------------//
  // Button control to swich on/off light.
  lcdLight = digitalRead(lcdLight_ButtonPin);   // Read button.
  delay(100);   //Avoid too fast button.
  
  if ( lcdLight == HIGH ) lcdLight_log=!lcdLight_log;
  
  if ( lcdLight_log ) lcd.backlight();
  else lcd.noBacklight();
  
  //----------------------------------------------------------------------//
  // Change watering time interval.
  ButtonIntervalWateringState = digitalRead(timeIntervalButtonPin);   // Read button.
  if (ButtonIntervalWateringState == HIGH){
    TimeIntervalWatering += 2;   // Change counter.
    if (TimeIntervalWatering >= 58) TimeIntervalWatering = 0;   // Watering time: Maximum is 58s.
    timeIntervalWatering_M = TimeIntervalWatering/60;   // Counter to min and sec.
    timeIntervalWatering_S = TimeIntervalWatering-timeIntervalWatering_M*60;
    
    Serial.print("Watering time = ");
    Serial.print(timeIntervalWatering_M);
    Serial.print("m ");
    Serial.print(timeIntervalWatering_S);
    Serial.println("s");
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.write("Watering time = ");
    lcd.setCursor(0,1);
    lcd.print(timeIntervalWatering_M);
    lcd.write("m");
    lcd.print(timeIntervalWatering_S);
    lcd.write("s / Max 50s");
    
    delay(100);   //Avoid too fast button.
  }
  
  //----------------------------------------------------------------------//
  // Switch on air pumping.
  for(int k=0;k<pump_per_day;k++){
    time_pump_min = floor(1.0+(24.0/pump_per_day)*float(k)*60);
    if ( timeRTC_H*60+timeRTC_M >= time_pump_min && timeRTC_H*60+timeRTC_M <= time_pump_min + period_pump_air_min 
         && digitalRead(airpump_relay_pin)==LOW )
      digitalWrite(airpump_relay_pin,HIGH);
  }
  
  //----------------------------------------------------------------------//
  // Watering sunrise.
  if ( ( timeRTC_H*60 + timeRTC_M == timeWater_H*60 + timeWater_M ) && hum_max_today<hum_rain ) {
    
    // Start watering: Open valve.
    Serial.println("Open valve");
    digitalWrite(relayPin, HIGH);
    digitalWrite(greenledPin, HIGH);
    
    Serial.println("Watering ---> ... ");
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.write("Watering ... ");
    
    // Watering during a specific time.
    timeWatering = millis();
    while ( millis()-timeWatering < TimeIntervalWatering*1000 ){
      
      Serial.print("                  ");
      Serial.print((millis()-timeWatering)/1000);
      Serial.print("s / ");
      Serial.print(TimeIntervalWatering);
      Serial.println("s");
      
      lcd.setCursor(0,1);
      lcd.print((millis()-timeWatering)/1000);
      lcd.write("s / ");
      lcd.print(TimeIntervalWatering);
      lcd.write("s");
      
      delay(1000);
    }
    
    // Finish watering: Close valve.
    lcd.clear();
    Serial.println("Close valve");
    digitalWrite(relayPin, LOW);
    digitalWrite(greenledPin, LOW);
    
    // Avoid reenter in the watering if TimeIntervalWatering is less than one minute.
    while ( (RTC.now().hour()==timeWater_H && RTC.now().minute()==timeWater_M) ) delay(1000);
  }
  
  //----------------------------------------------------------------------//
  // Watering sunset.
  if ( ( timeRTC_H*60 + timeRTC_M == timeWater2_H*60 + timeWater2_M ) && hum_max_today<hum_rain ) {
    
    // Start watering: Open valve.
    Serial.println("Open valve");
    digitalWrite(relayPin, HIGH);
    digitalWrite(greenledPin, HIGH);
    
    Serial.println("Watering ---> ... ");
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.write("Watering ... ");
    
    // Watering during a specific time.
    timeWatering = millis();
    while ( millis()-timeWatering < TimeIntervalWatering*1000 ){
      Serial.print("                  ");
      Serial.print((millis()-timeWatering)/1000);
      Serial.print("s / ");
      Serial.print(TimeIntervalWatering);
      Serial.println("s");
      
      lcd.setCursor(0,1);
      lcd.print((millis()-timeWatering)/1000);
      lcd.write("s / ");
      lcd.print(TimeIntervalWatering);
      lcd.write("s");
      
      delay(1000);
    }
    
    // Finish watering: Close valve.
    lcd.clear();
    Serial.println("Close valve");
    digitalWrite(relayPin, LOW);
    digitalWrite(greenledPin, LOW);
    
    // Avoid reenter in the watering if TimeIntervalWatering is less than one minute.
    while ( (RTC.now().hour()==timeWater2_H && RTC.now().minute()==timeWater2_M) ) delay(1000);
  }
  
  //----------------------------------------------------------------------//
  // Switch off air pumping.
  pump = false;
  for(int k=0;k<pump_per_day;k++){
    time_pump_min = floor(1.0+(24.0/pump_per_day)*float(k)*60);
    if ( timeRTC_H*60+timeRTC_M >= time_pump_min && timeRTC_H*60+timeRTC_M <= time_pump_min + period_pump_air_min ) pump = true;
  }
  
  if ( !pump && digitalRead(airpump_relay_pin)==HIGH ) digitalWrite(airpump_relay_pin,LOW);
  
}























