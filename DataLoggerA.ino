/*  TITULO: Data Logger V1 SD y RTC.
 
    AUTOR:
   
    Carlos Fernández (2018) --> INGENIERO ELECTRÓNICA Y COMUNICACIONES
    - FACEBOOK: https://www.facebook.com/Charlie1976
    - TWITTER: https://twitter.com/ventasarduinopa
    - CORREO: abacab1@hotmail.com
    - WEB: http://www.ventasarduinopanama.com/
   
    DESCRIPCIÓN DEL PROGRAMA
   
    Con este programa hacemos un Login para entrar a una pagina web (Seguridad Informatica).
   
   
    ESQUEMA DE CONEXION
   
                                      +-----+
         +----[PWR]-------------------| USB |--+
         |                            +-----+  |
         |         GND/RST2  [ ][ ]            |
         |       MOSI2/SCK2  [ ][ ]  A5/SCL[ ] | I2C LCD 2x16
         |          5V/MISO2 [ ][ ]  A4/SDA[ ] | I2C LCD 2x16
         |                             AREF[ ] |
         |                              GND[ ] |
         | [ ]N/C                    SCK/13[ ] |  SPI SCK
         | [ ]IOREF                 MISO/12[ ] |  SPI MISO
         | [ ]RST                   MOSI/11[ ]~|  SPI MOSI
         | [ ]3V3    +---+               10[ ]~| Chip Select 
    VCC  | [ ]5v    -| A |-               9[ ]~|  
    GND  | [ ]GND   -| R |-               8[ ] |   
         | [ ]GND   -| D |-                    |
         | [ ]Vin   -| U |-               7[ ] |  
         |          -| I |-               6[ ]~|  
         | [ ]A0    -| N |-               5[ ]~|  
         | [ ]A1    -| O |-               4[ ] |  
         | [ ]A2     +---+           INT1/3[ ]~|  
         | [ ]A3                     INT0/2[ ] |  
RTC      | [ ]A4/SDA  RST SCK MISO     TX>1[ ] |  
RTC      | [ ]A5/SCL  [ ] [ ] [ ]      RX<0[ ] |  
         |            [ ] [ ] [ ]              |
         |  MEGA_R3  GND MOSI 5V  ____________/
          \_______________________/
 
  NOTAS:
 
   - Cátodo(-) del LED (pata más corta) a GND a través de una R=220 omhs.
   - Ethernet Shield colocada sobre Arduino MEGA (Los pines coinciden al 100%).
   - Ethernet Shield conectada al router WIFI mediante cable Ethernet standard (conector RJ45)
   - Arduino UNO alimentado a través de una batería externa de Litio-ion de 12V/3000mAh (DC 12300)
*/

#include <SD.h>
#include <DHT.h>
#include <Wire.h>
#include "Sodaq_DS3231.h"   // A5 SCL - A4 SDA
#define DHTPIN1 2           // Pin 2 DHT
#define DHTTYPE DHT22       // Sensor DHT22
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // set the LCD address to 0x27 for a 16 chars and 2 line display
const int chipSelect = 10;  // SD card pin selecionado
//byte posicion=0;
DHT dht1(DHTPIN1, DHTTYPE);
File data_file;
File root;
int Espacio;                // Variable que nos dice la posicion de la coma en el mensaje.
String StringOne, StringTwo;
String stringThree;
unsigned long filename;
unsigned long interval=5000;      // el tiempo que necesita esperar
unsigned long previousMillis=20;  // millis() retornoa un unsigned long.
unsigned long intervalotime=500;      // tiempo de espera para imprimir LCD
unsigned long previousMillistime=20;  // millis() retorna un unsigned long.
unsigned int tstart;

byte smile[8] = {
  0b00000000,
  0b00001010,
  0b00001010,
  0b00001010,
  0b00000000,
  0b00010001,
  0b00001110,
  0b00000000,
};

void setup() 
{
  // set the initial time here:
  //cls();
  lcd.clear();
  lcd.begin(16, 2);
  lcd.createChar (0, smile);
  lcd.backlight();
  lcd.setCursor(2, 0);
  lcd.print("Data Logger");
  lcd.setCursor(2, 1);
  lcd.print("Abacab Corp");
  Wire.begin();
  Serial.begin(2000000);
  tstart=millis();
  rtc.begin();
  dht1.begin(); //This command will start to receive the values from dht22
      // SD Card Initialization
  pinMode(chipSelect, OUTPUT);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  if(!SD.begin(chipSelect))
  {
    Serial.println("Se ha producido un fallo al iniciar la comunicación SD");
    return;
  } 
    Serial.println("Se ha iniciado la comunicación correctamente");
    root = SD.open("/");
    delay(2000);
    //cls();
    lcd.clear();
}


void loop() 
{
  if (Serial.available()){
    //Lectura de caracteres   
   String data = Serial.readStringUntil('\n');
  do {
    Espacio = data.indexOf (" "); //Buscamos la primera coma del mensaje.
    if (Espacio != -1) {
      StringOne = data.substring (0, Espacio);
    data = data.substring (Espacio+1, data.length()); //Ahora borramos el mensaje y decimos que es desde la posicion posterior a la ‘,’ hasta el final del string.
    }
   else 
   { // Entramos aqui si ya no quedan mas ‘,’
    if (data.length() > 0)
    StringTwo = data;
     // Imprimimos el resto de texto que falta.
    }
    } 
   while (Espacio > 0 );
    {
    }    
  if (StringOne == "Type")
     {
      wait();
      LeeCSV(StringTwo);
     }
  else if (StringOne == "" && StringTwo == "dir")
    {
      //wait();
      printDirectory(root, 0);
      reset();
    }
  else if (StringTwo == "Reset")
  {
    reset();
  }

  else if (StringOne == "delete")
  {
    if (SD.exists(StringTwo)) 
   {
      SD.remove(StringTwo);
   }reset();
  }
    else {}
  } 

  unsigned long currentMillistime = millis(); // imprimir tiempo en la pantalla LCD 
 if ((unsigned long)(currentMillistime - previousMillistime) >= intervalotime) 
 {
  PrintLCDtime();
 }
      unsigned long currentMillis = millis(); // grab current time 
 if ((unsigned long)(currentMillis - previousMillis) >= interval) 
 {
          previousMillis = millis();
          DateTime now = rtc.now();
          filename = now.date();
          filename = filename * 100;
          filename = filename + now.month();
          filename = filename * 10000;
          filename = filename + now.year();
          stringThree = String(filename) + ".csv";
          data_file = SD.open(stringThree, FILE_WRITE);
   if (data_file)
    { 
      Serial.print("Escribiendo en file ");
      Serial.print(stringThree);
      Serial.print(" ....");
      PrintInfo();
      getData();
      PrintLCDTemp();
    }
    else {
    Serial.print("error abriendo ");
    Serial.print(stringThree);
    Serial.println(" ....");
 // if the file didn't open, print an error:
        }
 }
}

void getData()
{
      DateTime now = rtc.now();
      float temp = dht1.readTemperature(); //Reading the temperature as Celsius and storing in temp
      float h = dht1.readHumidity();       //Se lee la humedad
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
//Temperatura de piso    
    data_file.print(" Temp: ");
    data_file.print(",");
    data_file.print(temp);
    data_file.print(",");
    data_file.print(" Hum: ");
    data_file.print(",");
    data_file.println(h);
//Cierra el archivo
      data_file.flush();                          //saving the file
      data_file.close();                          //closing the file
}

void PrintInfo()
{
    DateTime now = rtc.now();
    float temp = dht1.readTemperature();          //Reading the temperature as Celsius and storing in temp
    float h = dht1.readHumidity();                //Se lee la humedad
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
//Temperatura de piso
    Serial.print(" Temp: ");
    Serial.print(temp);
    Serial.print(",");
    Serial.print(" Hum: ");
    Serial.println(h);
}

void PrintLCDtime()
{
    lcd.backlight();
    lcd.setCursor(0,0);
    DateTime now = rtc.now();
    lcd.print(now.hour(), DEC);
    lcd.print(":");
    lcd.print(now.minute(), DEC);
    lcd.print(":");
    lcd.print(now.second(), DEC);
    lcd.print(",");
    lcd.print(now.date(), DEC);
    lcd.print("/");
    lcd.print(now.month(), DEC);
    lcd.print("/");
    lcd.print(now.year()-2000, DEC);
}

void PrintLCDTemp()
{
    float temp = dht1.readTemperature();          //Reading the temperature as Celsius and storing in temp
    float h = dht1.readHumidity();                //Se lee la humedad
    lcd.backlight();
    lcd.setCursor(0,1);
    lcd.print("                 ");
//Temperatura de piso
    lcd.setCursor(0,1);
    lcd.print("T:");
    lcd.print(temp);
    //lcd.print("\377C");
    lcd.setCursor(8,1);
    //lcd.print(",");
    lcd.print("H:");
    lcd.print(h);
    lcd.println("%");
    }

String LeeCSV (String Archivo)
{ 
    data_file = SD.open(Archivo, FILE_WRITE);//"17062018.CSV"
     // volver a abrir el archivo para lectura:
    data_file = SD.open(Archivo);
   if (data_file) {
    // leer desde el archivo hasta que no haya nada más en el:
     while (data_file.available()) {
       Serial.write(data_file.read());
       
     }
    // cerrar el archivo:
       data_file.close();
   } else {
          // Si el archivo no se abre, imprimir un error:
            Serial.println("Error abiendo ");
            Serial.print(Archivo);
          } reset();
}

void printDirectory(File dir, int numTabs) 
{
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}
/*
void cls()
{
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("                 ");
    lcd.setCursor(0, 1);
    lcd.print("                 ");
}
*/
void reset()
{
   if ( (millis()-tstart)>1000 )
   {
     //Serial.println("Un segundo! Reseteando");  
     asm("jmp 0x0000"); 
   }
}

void wait()
  {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" Please wait. ");
      lcd.write((byte)0);
      lcd.setCursor(0,1);
      lcd.print("Processing..");
      lcd.print("%");
  }

