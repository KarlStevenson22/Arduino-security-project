/*
 * Example of getting your location, using the MTK3339.
 * It is based heavily on the examples provided by Adafruit
 *
 * Do whatever you like with this code.
 * A link on your website/blog to my blog is very much appreciated!
 */
  
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
 
SoftwareSerial gpsSerial(3, 2);
 
Adafruit_GPS GPS(&gpsSerial);
 
uint32_t timer = millis();
 
const int gpsEnablePin = 7;
 
void setup()  
{
  pinMode(gpsEnablePin, OUTPUT);
  
  Serial.begin(115200);
  GPS.begin(9600);
  
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  
  // 1 Hz update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
     
  // http://forums.adafruit.com/viewtopic.php?f=22&t=41452
  digitalWrite(gpsEnablePin, LOW); // turn GPS off
  
  delay(5000);
  
  digitalWrite(gpsEnablePin, HIGH); // turn GPS on
  
  delay(1000);
}
 
void loop()
{
  char c = GPS.read();
  
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }
  
  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();
  
  // approximately every 5 seconds or so, print out the current stats
  if (millis() - timer > 5000) {
    timer = millis(); // reset the timer
     
    Serial.print("\nTime: ");
    Serial.print(GPS.hour, DEC); Serial.print(':');
    Serial.print(GPS.minute, DEC); Serial.print(':');
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    Serial.print(GPS.milliseconds);
    Serial.println(" UTC");
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('-');
    Serial.print(GPS.month, DEC); Serial.print("-20");
    Serial.println(GPS.year, DEC);
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
     
    if (GPS.fix) {
      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
       
      //Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Speed (km/h): "); Serial.println(GPS.speed * 1.852);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      //Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
    }
    else {
      Serial.println("No sat fix :-(");
    }
  }
}
