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
bool is_time_to_pump;
int period_pump_air_min = 3;  // XXX min of air pumping every 24/pump_per_day (pump_per_day times per day).
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
unsigned long time_water_morning_H, time_water_morning_M;  // Sunrise watering time.
unsigned long time_water_evening_H, time_water_evening_M;  // Sunset watering time.

// Button variables to define watering time.
const int lcdLight_ButtonPin = 5;
int lcdLight = 0;
bool lcdLight_log = false;

// Button variables to define watering interval.
const int timeIntervalButtonPin = 9;
int ButtonIntervalWateringState;
unsigned long period_watering_sec = 56;   // Time of watering in seconds.
int period_watering_M, period_watering_S;

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
  
  if (!RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  
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
  time_water_morning_H = sun.sunrise.hour;
  time_water_morning_M = sun.sunrise.min;
  
  time_water_evening_H = sun.sunset.hour;
  time_water_evening_M = sun.sunset.min;
  time_water_evening_H -= 2;   // Before sunset.

//  // Test mode.
//  time_water_morning_H = 0;
//  time_water_morning_M = 0;
//  time_water_evening_H = 19;
//  time_water_evening_M = 00;
  
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
    if ( timeRTC_H*60 + timeRTC_M < time_water_morning_H*60 + time_water_morning_M ){
      Serial.print("TW = ");
      Serial.print(time_water_morning_H);
      Serial.print("h");
      Serial.print(time_water_morning_M);
      Serial.println("m");
      Serial.println();
    }
    else if ( timeRTC_H*60 + timeRTC_M < time_water_evening_H*60 + time_water_evening_M ){
      Serial.print("TW = ");
      Serial.print(time_water_evening_H);
      Serial.print("h");
      Serial.print(time_water_evening_M);
      Serial.println("m");
      Serial.println();
    }
    else{
      // Compute the next day watering time.
      sun.day += 1;
      sun.calculate();
      time_water_morning_H = sun.sunrise.hour;
      time_water_morning_M = sun.sunrise.min;
      Serial.print("TW = ");
      Serial.print(time_water_morning_H);
      Serial.print("h");
      Serial.print(time_water_morning_M);
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
    if ( timeRTC_H*60 + timeRTC_M < time_water_morning_H*60 + time_water_morning_M ){
      lcd.write("TW=");
      lcd.print(time_water_morning_H);
      lcd.write("h");
      lcd.print(time_water_morning_M);
      lcd.write("m");
    }
    else if ( timeRTC_H*60 + timeRTC_M < time_water_evening_H*60 + time_water_evening_M ){
      lcd.write("TW=");
      lcd.print(time_water_evening_H);
      lcd.write("h");
      lcd.print(time_water_evening_M);
      lcd.write("m");
    }
    else{
      // Compute the next day watering time.
      sun.day += 1;
      sun.calculate();
      time_water_morning_H = sun.sunrise.hour;
      time_water_morning_M = sun.sunrise.min;
      lcd.write("TW=");
      lcd.print(time_water_morning_H);
      lcd.write("h");
      lcd.print(time_water_morning_M);
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
    period_watering_sec += 2;   // Change counter.
    if (period_watering_sec >= 90) period_watering_sec = 0;   // Watering time: Maximum is 90s.
    period_watering_M = period_watering_sec/60;   // Counter to min and sec.
    period_watering_S = period_watering_sec-period_watering_M*60;
    
    Serial.print("Watering time = ");
    Serial.print(period_watering_M);
    Serial.print("m ");
    Serial.print(period_watering_S);
    Serial.println("s");
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.write("Watering time = ");
    lcd.setCursor(0,1);
    lcd.print(period_watering_M);
    lcd.write("m");
    lcd.print(period_watering_S);
    lcd.write("s / Max 90s");
    
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
  if ( ( timeRTC_H*60 + timeRTC_M == time_water_morning_H*60 + time_water_morning_M ) && hum_max_today<hum_rain ) {
    
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
    while ( millis()-timeWatering < period_watering_sec*1000 ){
      
      Serial.print("                  ");
      Serial.print((millis()-timeWatering)/1000);
      Serial.print("s / ");
      Serial.print(period_watering_sec);
      Serial.println("s");
      
      lcd.setCursor(0,1);
      lcd.print((millis()-timeWatering)/1000);
      lcd.write("s / ");
      lcd.print(period_watering_sec);
      lcd.write("s");
      
      delay(1000);
    }
    
    // Finish watering: Close valve.
    lcd.clear();
    Serial.println("Close valve");
    digitalWrite(relayPin, LOW);
    digitalWrite(greenledPin, LOW);
    
    // Avoid reenter in the watering if period_watering_sec is less than one minute.
    while ( (RTC.now().hour()==time_water_morning_H && RTC.now().minute()==time_water_morning_M) ) delay(1000);
  }
  
  //----------------------------------------------------------------------//
  // Watering sunset.
  if ( ( timeRTC_H*60 + timeRTC_M == time_water_evening_H*60 + time_water_evening_M ) && hum_max_today<hum_rain ) {
    
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
    while ( millis()-timeWatering < period_watering_sec*1000 ){
      Serial.print("                  ");
      Serial.print((millis()-timeWatering)/1000);
      Serial.print("s / ");
      Serial.print(period_watering_sec);
      Serial.println("s");
      
      lcd.setCursor(0,1);
      lcd.print((millis()-timeWatering)/1000);
      lcd.write("s / ");
      lcd.print(period_watering_sec);
      lcd.write("s");
      
      delay(1000);
    }
    
    // Finish watering: Close valve.
    lcd.clear();
    Serial.println("Close valve");
    digitalWrite(relayPin, LOW);
    digitalWrite(greenledPin, LOW);
    
    // Avoid reenter in the watering if period_watering_sec is less than one minute.
    while ( (RTC.now().hour()==time_water_evening_H && RTC.now().minute()==time_water_evening_M) ) delay(1000);
  }
  
  //----------------------------------------------------------------------//
  // Switch off air pumping.

  // Is pumping?
  is_time_to_pump = false;
  for(int k=0;k<pump_per_day;k++){
    time_pump_min = floor(1.0+(24.0/pump_per_day)*float(k)*60);
    if ( timeRTC_H*60+timeRTC_M >= time_pump_min && timeRTC_H*60+timeRTC_M <= time_pump_min + period_pump_air_min ) is_time_to_pump = true;
  }

  // If it is pumping but time to pump have finished, we close the pumper.
  if ( !is_time_to_pump && digitalRead(airpump_relay_pin)==HIGH ) digitalWrite(airpump_relay_pin,LOW);
  
}


