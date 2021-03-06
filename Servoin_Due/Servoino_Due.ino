#include <LiquidCrystal.h> //Include the LCD library
#include <Servo.h>

// constants won't change. They're used here to 
// set pin numbers:
const int receiverPin = 7;
int receiverState;
int flag = 0;      //'flag' variable, used to indicate whether the intial message has already been printed

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // these are the pins connected to the display.

Servo lockServo;

void setup() //Runs once when the board is powered up
{  
  
  pinMode(receiverPin, INPUT);
  //How columns, rows
  lcd.begin(16,2);  
  //start with blank screen
  lcd.clear(); 
  //set cursor to column 0, row 0
  lcd.setCursor(0,0);

  lockServo.attach(8);
  delay(1800);  
  lockServo.write(180);
}

// End of the setup. Now moving to the loop, the main function of Arduino.

void loop()
{  

  receiverState = digitalRead(receiverPin);  // read the state of the pushbutton value:
  
  if (receiverState == LOW)
  { 
    //Reset the LCD display

    lcd.begin(16,2);  //columns, rows
    lcd.setCursor(0,0);  //set cursor to column 0, row 0
  
    lcd.print("ACCESS DENIED");
    lockServo.write(160);
    delay(1000);
  }
  else
  {
    lcd.begin(16,2);  //columns, rows
    lcd.setCursor(0,0);  //set cursor to column 0, row 0
       
    lcd.print("ACCESS GRANTED");
    lockServo.write(70);
    delay(10000);
  }
}






