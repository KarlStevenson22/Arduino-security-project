#include <Wire.h>
#include <ADXL345.h>

ADXL345 adxl;    // Variable adxl is an instance of the ADXL345 library
 
// Declaring pins
const int Button_Pin = 2;
const int WLED_Pin = 8;
const int GLED1_Pin = 9;
const int GLED2_Pin = 10;
const int GLED3_Pin = 11;
const int RLED_Pin = 12;

// Declaring center values and thresholds for x, y and z sensitivity
int button;
int is_init;             // Initialisation status at rest
int is_latch;            // LED latch
int thold = 5;           // Sensitivity threshold 
int x_lock = 20;         // Lock values
int y_lock = 40;
int z_lock = 15;
int offset[3] = {0};     // x, y and z offsets i.e. value at rest 
float gravi = 0.0392;    // 4mg/LSB, replace with 0.0392 if needbe
  
void setup()
{   
  // Set data rate.
  Serial.begin(9600);
  
  // Initialising addons
  button = digitalRead(Button_Pin);
  adxl.powerOn();
  
  // Declaring pins
  pinMode(WLED_Pin, OUTPUT);
  pinMode(GLED1_Pin, OUTPUT);
  pinMode(GLED2_Pin, OUTPUT);
  pinMode(GLED3_Pin, OUTPUT);
  pinMode(RLED_Pin, OUTPUT);
  pinMode(Button_Pin, INPUT);

  // Resetting status vars
  is_init = 0;         
  is_latch = 0;
}
  
void loop()
{ 
  // Declaring measured params
  int count;
  int x, y, z;             // Measured values
  int gx, gy, gz;          // Converted values
  int x_angle;             // Measured angles            
  int y_angle;
  
  // Reading x, y, z values
  adxl.readAccel(&x, &y, &z);
  
  // Initialisation at rest
  if (is_init == 0)
  { 
    for (count = 0; count < 20; count + 1)
    {
      Serial.println("Initialising...");
      offset[1] = offset[1] + x;
      offset[2] = offset[2] + y;
      offset[3] = offset[3] + z;
    }
    offset[1] = offset[1]/20;
    offset[2] = offset[2]/20;
    offset[3] = offset[3]/20;   
    is_init = 1;
    digitalWrite(WLED_Pin, LOW);
    Serial.println("Done!");
  }
  // Soft reset
  else if (button == HIGH)
  {
    is_init = 0;
    digitalWrite(GLED1_Pin, LOW);
    digitalWrite(GLED2_Pin, LOW);
    digitalWrite(GLED3_Pin, LOW);
    digitalWrite(RLED_Pin, HIGH);
    digitalWrite(WLED_Pin, HIGH);
  }
  // Running normal operation
  else 
  {
    // Converting increments
    gx = (x-offset[1])*gravi;
    gy = (y-offset[2])*gravi;
    gz = (z)*gravi;
    
    // Calculating pitch and roll
    x_angle = atan(gx/gz)*57.296;     // 57.296 converting to degrees from radians
    y_angle = atan(gy/gz)*57.296;
    
    // Printing results to serial monitor
    Serial.print("Pitch...");
    Serial.println(y_angle);
    Serial.print("Roll...");
    Serial.println(x_angle);
    Serial.print("Gforce...");
    Serial.println(gz);
  }
      
}

