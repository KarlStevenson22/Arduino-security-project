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
int button = 0;
int is_init;                          // Initialisation status at rest
int is_latch;                         // LED latch
int thold = 10;                       // Sensitivity threshold 
int x_lock[3] = {20, -30, 5};         // Lock values
int y_lock[3] = {40, 20, 70};
int z_lock[3] = {9, 9, 3};
float gravi;                          // 4mg/LSB, replace with 0.0392 if needbe
int offset[3];                        // x, y and z offsets i.e. value at rest 
  
void setup()
{   
  // Set data rate.
  Serial.begin(9600);
  adxl.powerOn();
 
  // Declaring pins
  pinMode(WLED_Pin, OUTPUT);
  pinMode(GLED1_Pin, OUTPUT);
  pinMode(GLED2_Pin, OUTPUT);
  pinMode(GLED3_Pin, OUTPUT);
  pinMode(RLED_Pin, OUTPUT);
  pinMode(Button_Pin, INPUT);

  // Resetting status vars
  gravi = 0;
  is_init = 0;         
  is_latch = 0;
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
  
  // Reading x, y, z values
  adxl.readAccel(&x, &y, &z);
  //Serial.println(x);
  //Serial.println(y);
  //Serial.println(z);
    
  // Initialising addons
  button = digitalRead(Button_Pin);
  
  // Initialisation at rest
  if (is_init == 0)
  { 
    Serial.println("Initialising...");
    digitalWrite(WLED_Pin, HIGH);
    digitalWrite(RLED_Pin, HIGH);
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
    tone(3, 350, 2000);
    delay(2000);
    digitalWrite(WLED_Pin, LOW);
    Serial.println("Done!");
  }
  // Soft reset
  else if (button == HIGH)
  {
    is_init = 0;
    is_latch = 0;
    digitalWrite(GLED1_Pin, LOW);
    digitalWrite(GLED2_Pin, LOW);
    digitalWrite(GLED3_Pin, LOW);
    tone(3, 750, 2000);
  }
  // Running normal operation
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
    if (gz > z_lock[0] && (x_angle < x_lock[0] + thold && x_angle > x_lock[0] - thold) && (y_angle < y_lock[0] + thold && y_angle > y_lock[0] - thold))
    {
      digitalWrite(GLED1_Pin, HIGH);
      is_latch = 1;
    }
    else if (is_latch == 1 && gz > z_lock[1] && (x_angle < x_lock[1] + thold && x_angle > x_lock[1] - thold) && (y_angle < y_lock[1] + thold && y_angle > y_lock[1] - thold))
    {
      digitalWrite(GLED2_Pin, HIGH);
      is_latch = 2;    
    }
    else if (is_latch == 2 && gz > z_lock[2] && (x_angle < x_lock[2] + thold && x_angle > x_lock[2] - thold) && (y_angle < y_lock[2] + thold && y_angle > y_lock[2] - thold))
    {
      digitalWrite(GLED3_Pin, HIGH);
      digitalWrite(RLED_Pin, LOW);
      is_latch = 3;
    }
  }
  Serial.print("Pitch...: ");
  Serial.println(y_angle);
  Serial.print("Roll...: ");
  Serial.println(x_angle);
  Serial.print("G-Force...:");
  Serial.println(gz);
  delay(500);      
}

