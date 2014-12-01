#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <ADXL345.h>

ADXL345 adxl;
 
SoftwareSerial gpsSerial(3, 2);

Adafruit_GPS GPS(&gpsSerial);

// The timer is used to control the GPS output.
uint32_t timer = millis();

const int gpsEnablePin = 7;

// Declaring LED pin variables
const int GLED1 = 8;
const int GLED2 = 9;
const int GLED3 = 10;
const int GLED4 = 11;
const int RLED = 12;

// Lock constants

// Time
const int hourLock = 16;
const int minLock = 55;
const int minThresh = 5;

// Date
const int dayLock = 1;
const int monLock = 12;
const int yearLock = 14;

// Position
const int latLock = 5400;
const int longLock = 114;
const int posThresh = 200;

// Satelites
const int satLock = 100;
const int satThresh = 100;

// Accelerometer
int thold = 20;                     // Sensitivity threshold 
int x_lock[3] = {0, 45, -45};        // x_axis angle lock values
int y_lock[3] = {0, 0, 0};        // y_axis angle lock values
int z_lock[3] = {10, 5, 5};          // acceleration lock values

// Stage Number
int stage;

// Timer
int timerval;

// Calculation and stage constants
int is_init;
float gravi;                         // 4mg/LSB, replace with 0.0392 if needbe
int offset[3];                       // x, y and z offsets i.e. value at rest 

void setup()
{
  // Stage reset
  stage = 0;
  
  // Setting up pins
  pinMode(GLED1, OUTPUT);
  pinMode(GLED2, OUTPUT);
  pinMode(GLED3, OUTPUT);
  pinMode(GLED4, OUTPUT);
  pinMode(RLED, OUTPUT);

  // Resetting all LEDs
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);  
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  digitalWrite(12, HIGH);
  
  pinMode(gpsEnablePin, OUTPUT);
  
  Serial.begin(115200);
  
  // GPS setup
  GPS.begin(9600); 
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);  
  // 1 Hz update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);  
  
  // Pin declarations
  digitalWrite(gpsEnablePin, LOW); // Turn GPS off
  delay(5000)  
  digitalWrite(gpsEnablePin, HIGH); // Turn GPS on 
  delay(1000);
  
  // Accelerometer setup
  adxl.powerOn();
  
  gravi = 0;
  is_init = 0;
}

void loop()
{  
  
  // Declaring measured params
  int count;
  int x, y, z;             // Measured values
  float gx = 0.0;
  float gy = 0.0; 
  float gz = 0.0;          // Converted values
  float x_angle = 0.0;     // Measured angles            
  float y_angle = 0.0;
   
  char c = GPS.read();

  // In case the message cannot be parsed
  if (GPS.newNMEAreceived()) 
  {
    if (!GPS.parse(GPS.lastNMEA()))
      return;  
  }
  
  // Keeps the timerino in check
  if (timer > millis())  timer = millis();
 
  // Sets timers
  if (stage <= 7)
  {
    timerval = 3000;
  }
  else 
  {
    timerval = 500;
  }
 
  // Keeps timers in check
  if (millis() - timer > timerval) 
  {
    timer = millis(); // Resets timer for the next loop
    
    // Outputs of the GPS readings
    if (stage <= 7)
    {
      Serial.print("\nTime: ");
      Serial.print(GPS.hour, DEC); Serial.print(':');
      Serial.print(GPS.minute, DEC); Serial.print(':');
      Serial.print(GPS.seconds, DEC); Serial.print('.');
      Serial.print("Date: ");
      Serial.print(GPS.day, DEC); Serial.print('/');
      Serial.print(GPS.month, DEC); Serial.print("/20");
      Serial.println(GPS.year, DEC);
      Serial.print("Fix: "); Serial.print((int)GPS.fix);
      Serial.print(" quality: "); Serial.println((int)GPS.fixquality); 
      Serial.print("Stage: "); Serial.println(stage);
    }
    if ((GPS.fix == 1) && (stage <= 7)) 
    {
      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", "); 
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
    }
    else if ((GPS.fix == 0) && (stage <= 7))
    {    
      Serial.println("No sat fix :-(");
    }
     
    // Security stage conditionals
    if ((stage == 0) && (GPS.hour == hourLock))
    {
      Serial.println("Hour is check!");
      digitalWrite(8, HIGH);
      digitalWrite(9, LOW);  
      digitalWrite(10, LOW);
      digitalWrite(11, LOW);
      stage = 1;
    }
    else if ((stage == 1) && (GPS.minute > minLock - minThresh) && (GPS.minute < minLock + minThresh))
    {
      Serial.println("Minutes are check!");
      digitalWrite(8, LOW);
      digitalWrite(9, HIGH);  
      digitalWrite(10, LOW);
      digitalWrite(11, LOW);
      stage = 2;
    }
    else if ((stage == 2) && (GPS.day == dayLock))
    {
      Serial.println("Day is check!"); 
      digitalWrite(8, HIGH);
      digitalWrite(9, HIGH);  
      digitalWrite(10, LOW);
      digitalWrite(11, LOW);
      stage = 3;
    }
    else if ((stage == 3) && (GPS.month == monLock))
    {
      Serial.println("Month is check!");
      digitalWrite(8, LOW);
      digitalWrite(9, LOW);  
      digitalWrite(10, HIGH);
      digitalWrite(11, LOW);
      stage = 4;
    }
    else if ((stage == 4) && (GPS.year == yearLock))
    {
      Serial.println("Year is checkerino!");
      digitalWrite(8, HIGH);
      digitalWrite(9, LOW);  
      digitalWrite(10, HIGH);
      digitalWrite(11, LOW);
      stage = 5;
    }
    else if ((stage == 5) && (GPS.satellites > satLock - satThresh) && (GPS.satellites < satLock + satThresh))
    {
      Serial.println("Number of satellites is check!");
      digitalWrite(8, LOW);
      digitalWrite(9, HIGH);  
      digitalWrite(10, HIGH);
      digitalWrite(11, LOW);
      stage = 6;
    }
    else if ((stage == 6) && (GPS.latitude > latLock - posThresh) && (GPS.latitude < latLock + posThresh))
    {
      Serial.println("Latitude is check!");
      digitalWrite(8, HIGH);
      digitalWrite(9, HIGH);  
      digitalWrite(10, HIGH);
      digitalWrite(11, LOW);
      stage = 7;
    }
    else if ((stage == 7) && (GPS.longitude > longLock - posThresh) && (GPS.longitude < longLock + posThresh))
    {
      Serial.println("Longitude is check!");
      digitalWrite(8, LOW);
      digitalWrite(9, LOW);  
      digitalWrite(10, LOW);
      digitalWrite(11, HIGH);
      stage = 8;
    }
    else if ((stage >= 8) && (stage <= 10))
    {
      Serial.println("Location verified, proceeding to the next phase...");
      
      adxl.readAccel(&x, &y, &z);
      
      if (is_init == 0)
      { 
        Serial.println("Initialising movement recognition...");
        delay(2500);
        offset[0] = 0;
        offset[1] = 0;
        offset[2] = 0;  
        for (count = 0; count < 20; count++)
        {
          offset[0] = offset[0] + x;
          offset[1] = offset[1] + y;
          offset[2] = offset[2] + z;
        }
        offset[0] = offset[0]/20;
        offset[1] = offset[1]/20;
        offset[2] = offset[2]/20;  
        gravi = 9.81/offset[2];
        is_init = 1;
        delay(2000);
        Serial.println("Done!");
      }
      else
      {
        // Converting increments
        gx = (float)(x-offset[0])*gravi;
        gy = (float)(y-offset[1])*gravi;
        gz = (float)z*gravi;
          
        // Calculating pitch and roll
        x_angle = atan(gx/gz)*57.296;     // 57.296 converting to degrees from radians
        y_angle = atan(gy/gz)*57.296;
        
        // Lock sequence
        if (stage == 8 && (gz > z_lock[0]) && (x_angle < x_lock[0] + thold && x_angle > x_lock[0] - thold) && (y_angle < y_lock[0] + thold && y_angle > y_lock[0] - thold))
        {
          digitalWrite(8, HIGH);
          digitalWrite(9, LOW);  
          digitalWrite(10, LOW);
          digitalWrite(11, HIGH);
          stage = 9;
        }
        else if (stage == 9 && gz > z_lock[1] && (x_angle < x_lock[1] + thold && x_angle > x_lock[1] - thold) && (y_angle < y_lock[1] + thold && y_angle > y_lock[1] - thold))
        {
          digitalWrite(8, LOW);
          digitalWrite(9, HIGH);  
          digitalWrite(10, LOW);
          digitalWrite(11, HIGH);
          stage = 10;    
        }
        else if (stage == 10 && gz > z_lock[2] && (x_angle < x_lock[2] + thold && x_angle > x_lock[2] - thold) && (y_angle < y_lock[2] + thold && y_angle > y_lock[2] - thold))
        {
          digitalWrite(8, HIGH);
          digitalWrite(9, HIGH);  
          digitalWrite(10, LOW);
          digitalWrite(11, HIGH);
          stage = 11;
        }
      }  
      Serial.print("Pitch...: ");
      Serial.println(y_angle);
      Serial.print("Roll...: ");
      Serial.println(x_angle);
      Serial.print("G-Force...:");
      Serial.println(gz);      
    }
    else if (stage == 11)
    {
      digitalWrite(8, LOW);
      digitalWrite(9, LOW);  
      digitalWrite(10, LOW);
      digitalWrite(11, LOW);
      digitalWrite(12, LOW);
      Serial.println("ACCESS GRANTED!");
    }     
  }
}

