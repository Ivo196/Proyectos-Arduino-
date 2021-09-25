#include <Wire.h>
#include <LiquidCrystal.h> //Libreria del LCD
LiquidCrystal lcd(8,9,4,5,6,7); //Definimos los pines del LCD


#include "DHT.h" //cargamos la librería DHT
#define DHTPIN 23 //Seleccionamos el pin 23 en el que se conectará el sensor
#define DHTTYPE DHT11 //Se selecciona el DHT11 (hay //otros DHT)
DHT dht(DHTPIN, DHTTYPE); //Se inicia una variable que será usada por Arduino para comunicarse con el sensor


void setup()
{
    Serial.begin(9600); //Se incicia la comunicacion con el puerto serial
    Wire.begin();

    dht.begin(); //Se inicia el sensor de temperatura
    
    lcd.begin(16, 2); //Definimos las Dimenciones del LCD
    lcd.clear(); //Borramos pantalla 
}

void loop()
{
    float humedadTH = dht.readHumidity(); //Se lee la humedad
    float temperaturaTH = dht.readTemperature(); //Se lee la temperatura

//Imprimimos en el LCD

        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Tempera: ");
        lcd.print(int(temperaturaTH));
        lcd.print((char)223);
        lcd.print("C");
        
        lcd.setCursor(0,1);
        lcd.print("Humedad: ");
        lcd.print(int(humedadTH));
        lcd.print((char)37);

        delay(2000);
  
}
