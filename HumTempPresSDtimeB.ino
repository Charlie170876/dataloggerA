#include <SD.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include "Sodaq_DS3231.h" // A5 SCL - A4 SDA
#define DHTPIN1 7
#define DHTPIN2 2
#define DHTTYPE DHT11
#define BMP_SCK 10 // SCL/SCK
#define BMP_MISO 8 // SDO
#define BMP_MOSI 9 // SDA/SDI
#define BMP_CS 6   // CSB
DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);
File data_file;


Adafruit_BMP280 bme(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);

String stringTwo;
unsigned long filename;

void setup() 
{
  // set the initial time here:
  Wire.begin();
  Serial.begin(9600);
  Serial.println(F("BMP280 test"));
  rtc.begin();
 dht1.begin(); //This command will start to receive the values from dht22
 dht2.begin();
 //  SD Card Initialization
 if (!bme.begin()) {  
     Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
     //digitalWrite(A0, HIGH);
     delay(200);
     while (1);
   }
  pinMode(3, OUTPUT);
  //pinMode(A0,OUTPUT);
  if(!SD.begin(3))
  {
    Serial.println("Se ha producido un fallo al iniciar la comunicación SD");
    return;
  } 
    Serial.println("Se ha iniciado la comunicación correctamente");
}


void loop() 
{
          DateTime now = rtc.now();
          filename = now.date();
          filename = filename * 100;
          filename = filename + now.month();
          filename = filename * 10000;
          filename = filename + now.year();
          stringTwo = String(filename) + ".csv";
          data_file = SD.open(stringTwo, FILE_WRITE);
  if (data_file)
    { 
      Serial.print("Escribiendo en file ");
      Serial.print(stringTwo);
      Serial.print(" ....");
      PrintInfo();
      getData(); 
    }
    else {
    Serial.print("error abriendo ");
    Serial.print(stringTwo);
    Serial.println(" ....");
 // if the file didn't open, print an error:
        }delay(500);
}

void getData()
{
      DateTime now = rtc.now();
      float temp = dht1.readTemperature(); //Reading the temperature as Celsius and storing in temp
      float h = dht1.readHumidity();       //Se lee la humedad
      float h2 = dht2.readHumidity();      //Guarda la lectura de la humedad en la variable float h
      float t2 = dht2.readTemperature();   //Guarda la lectura de la temperatura en la variable float t
      
      data_file.print(now.hour(), DEC);
      data_file.print(":");
      data_file.print(now.minute(), DEC);
      data_file.print(":");
      data_file.print(now.second(), DEC);
      data_file.print(":");
      data_file.print(",");
      data_file.print(now.date(), DEC);
      data_file.print("/");
      data_file.print(now.month(), DEC);
      data_file.print("/");
      data_file.print(now.year(), DEC);
      data_file.print(",");
//Medidas internas      
      data_file.print(F("Temp = "));
      data_file.print(",");
      data_file.print(bme.readTemperature());
      data_file.print(" *C");
      data_file.print(",");
      data_file.print(F("Pres = "));
      data_file.print(",");
      data_file.print(bme.readPressure());
      data_file.print(" Pa");
      data_file.print(",");
      data_file.print(F("Aprox alt = "));   //prints the CO value
      data_file.print(",");
      data_file.print(bme.readAltitude(1013.25));
      data_file.print(" m");                      //prints the limit reached as either LOW or HIGH (above or underneath)
      data_file.print(",");
//Temperatura de piso    
    data_file.print(" Temp Piso: ");
    data_file.print(",");
    data_file.print(temp);
    data_file.print(",");
    data_file.print(" Hum Piso: ");
    data_file.print(",");
    data_file.print(h);
    data_file.print(",");
//Temperatura de Techo
    data_file.print(" Temp Techo: ");
    data_file.print(",");
    data_file.print(t2);
    data_file.print(",");
    data_file.print(" Hum Techo: ");
    data_file.print(",");
    data_file.println(h2);
//Cierra el archivo
      data_file.flush();                          //saving the file
      data_file.close();                          //closing the file
      delay(2000);                                //por ahora 2 segundos entre cada lectura
}

void PrintInfo()
{
    DateTime now = rtc.now();
    float temp = dht1.readTemperature();          //Reading the temperature as Celsius and storing in temp
    float h = dht1.readHumidity();                //Se lee la humedad
    float t2 = dht2.readTemperature();            //Reading the temperature as Celsius and storing in temp
    float h2 = dht2.readHumidity();               //Se lee la humedad
    //DateTime now = rtc.now();
    Serial.print(now.hour(), DEC);
    Serial.print(":");
    Serial.print(now.minute(), DEC);
    Serial.print(":");
    Serial.print(now.second(), DEC);
    Serial.print(",");
    Serial.print(now.date(), DEC);
    Serial.print("/");
    Serial.print(now.month(), DEC);
    Serial.print("/");
    Serial.print(now.year(), DEC);
    Serial.print(",");
//Medidas internas
    Serial.print(F(" Temp = "));
    Serial.print(bme.readTemperature());
    Serial.print(",");
    Serial.print(" *C");
    Serial.print(F(" Pres = "));
    Serial.print(bme.readPressure());
    Serial.print(" Pa");
    Serial.print(",");
    Serial.print(F(" Aprox alt = "));
    Serial.print(",");
    Serial.print(bme.readAltitude(1013.25));        // this should be adjusted to your local forcase
    Serial.print(" m");
    Serial.print(",");
//Temperatura de piso
    Serial.print(" Temp Piso: ");
    Serial.print(temp);
    Serial.print(",");
    Serial.print(" Hum Piso: ");
    Serial.print(h);
//Temperatura de Techo   
    Serial.print(" Temp Techo: ");
    Serial.print(t2);
    Serial.print(",");
    Serial.print(" Hum Techo: ");
    Serial.println(h2);
}

