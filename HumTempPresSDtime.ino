#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <DS3231.h>

#define BMP_SCK 10
#define BMP_MISO 8
#define BMP_MOSI 9 
#define BMP_CS 6
 
Adafruit_BMP280 bme(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);

DS3231 Clock;
byte year, month, date, DoW, hour, minute, second;
String stringTwo;
unsigned long filename;

void setup() 
{
  Serial.begin(9600);
   
  if (!bme.begin()) {  
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }
}
 
void loop() {
    Serial.print(F("Temperature = "));
    Serial.print(bme.readTemperature());
    Serial.println(" *C");
    
    Serial.print(F("Pressure = "));
    Serial.print(bme.readPressure());
    Serial.println(" Pa");
 
    Serial.print(F("Approx altitude = "));
    Serial.print(bme.readAltitude(1013.25)); // this should be adjusted to your local forcase
    Serial.println(" m");
    
    Serial.println();
    delay(2000);
}
