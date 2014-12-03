#include <Wire.h>
#include <ADXL345.h>


ADXL345 adxl; // Variable adxl is an instance of the ADXL345 library

// Declares all of the pins used

const int ButtonPin = 2;
const int LEDPin = 8;
const int GLEDPin = 9;
const int GLED2Pin = 10;
const int GLED3Pin = 11;
const int RLEDPin = 12;
int button = 0;

void setup(){
  
  // Sets the pins to input or output
  
  pinMode(LEDPin, OUTPUT);
  pinMode(GLEDPin, OUTPUT);
  pinMode(RLEDPin, OUTPUT);
  pinMode(GLED2Pin, OUTPUT);
  pinMode(GLED3Pin, OUTPUT);
  pinMode(ButtonPin, INPUT);
  
  Serial.begin(9600);
  adxl.powerOn();
}

void loop(){
    
  int x,y,z;
  int i;
  
  // Center values and thresholds for x,y and z sensitivity
  int tholdx = 100;
  int centerx = 13;
  
  int tholdy = 150;
  int centery_vert = -8;
  int centery_hori = 267;
  
  int tholdz_up = 50;
  int tholdz_down = 75;
  int centerz_vert = 280;
  
  int tholdz_left = 75;
  int tholdz_right = 75;
  int centerz_hori = 25;
  
  // Lock begins closed (obviously!!)
  
  boolean lock = false;
  
  while (lock == false)
  {
 
      button = digitalRead(ButtonPin);
      digitalWrite(LEDPin, HIGH);
      
      adxl.readAccel(&x, &y, &z); // Read the accelerometer values and store them in variables  x,y,z
      Serial.print("x============");
      Serial.println(x);
      Serial.print("y=======");
      Serial.println(y);
      Serial.print("z===");
      Serial.println(z);
      
      delay(100);
         
      if (button == LOW) // Nothing will happen if button is not pressed
      {
        digitalWrite(RLEDPin, HIGH);
        digitalWrite(GLEDPin, LOW);
        digitalWrite(GLED2Pin, LOW);
        digitalWrite(GLED3Pin, LOW);
        continue;
      }
      if ((z < centerz_vert - tholdz_down) && (x < centerx + tholdx) && (x > centerx - tholdx) && (y < centery_vert + tholdy) && (y > centery_vert - tholdy))
      {
        // Executes if accelerating downwards
        digitalWrite(GLEDPin, HIGH);
        digitalWrite(RLEDPin, LOW);
        tone(13,500,2000);
        delay(3000);
      }
      else if ((z > centerz_vert + tholdz_up) && (x < centerx + tholdx) && (x > centerx - tholdx) && (y < centery_vert + tholdy) && (y > centery_vert - tholdy))
      {
        // Executes if accelerating upwards
        digitalWrite(GLED2Pin, HIGH);
        digitalWrite(RLEDPin, LOW);
        tone(13,1000,2000);
        delay(3000);
      } 
      else if ((z > centerz_hori + tholdz_left) && (x < centerx + tholdx) && (x > centerx - tholdx) && (y < centery_hori + tholdy) && (y > centery_hori - tholdy))
      {
        // Executes if accelerating to the left (must be on its side)
        digitalWrite(GLED3Pin, HIGH);
        digitalWrite(RLEDPin, LOW);
        tone(13,1500,2000);
        delay(3000);      
      }
      else if ((z < centerz_hori - tholdz_right) && (x < centerx + tholdx) && (x > centerx - tholdx) && (y < centery_hori + tholdy) && (y > centery_hori - tholdy))
      {
        // Executes if accelerating to the right (must be on its side)
        digitalWrite(GLED3Pin, HIGH);
        digitalWrite(RLEDPin, LOW);
        tone(13,2000,2000);
        delay(3000);
        lock = true;
      }    
  }
  // Executes when the lock has been opened  
  digitalWrite(RLEDPin, HIGH);
  digitalWrite(GLEDPin, HIGH);
  digitalWrite(GLED2Pin, HIGH);
  digitalWrite(GLED3Pin, HIGH);
  delay(100000);
}
