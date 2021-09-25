#include <Wire.h>
#include <LiquidCrystal.h> //Libreria del LCD
LiquidCrystal lcd(8,9,4,5,6,7); //Definimos los pines del LCD


#include "DHT.h" //cargamos la librería DHT
#define DHTPIN 23 //Seleccionamos el pin 23 en el que se conectará el sensor
#define DHTTYPE DHT11 //Se selecciona el DHT11 (hay //otros DHT)
DHT dht(DHTPIN, DHTTYPE); //Se inicia una variable que será usada por Arduino para comunicarse con el sensor

#include <SPI.h> //Incluimos librerias
#include <Adafruit_Sensor.h> //Incluimos librerias BMP280
#include <Adafruit_BMP280.h> //Incluimos librerias BMP280

#define BMP_SCK 13 
#define BMP_MISO 12
#define BMP_MOSI 11
#define BMP_CS 10

Adafruit_BMP280 bmp; // I2C

//////////////////PINES///////////////////////
/* 
  BMP280
 VCC= 5v
 GND= GND
 SCL= 21(SCL)
 SDA= 20(SDA)

  DHT11
 += 5v
 -= GND
 OUT= 23
*/

/////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  dht.begin(); //Se inicia el sensor de temperatura
    
  lcd.begin(16, 2); //Definimos las Dimenciones del LCD
  lcd.clear(); //Borramos pantalla 

  if (!bmp.begin(0x76)) { //Canal 0x76 para I2C
    Serial.println(F("Fallo!!, REVISAR CONEXIÓN"));
    while (1);
  }
}

void loop() {

 float humedadTH = dht.readHumidity(); //Se lee la humedad
 float temperaturaTH = dht.readTemperature(); //Se lee la temperatura
    
 //Imprimimos en el LCD

        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Temp1: ");
        lcd.print(int(temperaturaTH));
        lcd.print((char)223);
        lcd.print("C");
        
        lcd.setCursor(0,1);
        lcd.print(F("Temp2: "));
        lcd.print(bmp.readTemperature());
        lcd.print((char)223);
        lcd.print("C");
      
        delay(2000);

        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(F("Pres: "));
        lcd.print(bmp.readPressure());
        lcd.print("Pa");
        
        lcd.setCursor(0,1);
        lcd.print("Humedad: ");
        lcd.print(int(humedadTH));
        lcd.print((char)37);
        
        delay(2000);

        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(F("  Altitud Aprox "));
        lcd.setCursor(0,1);
        lcd.print("     ");
        lcd.print(bmp.readAltitude(1032)); // Aquí colocar la presión en hPa del lugar donde vives.
        lcd.print(" m");

        delay(2000);
        
}


