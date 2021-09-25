#include <Wire.h>
#include <SPI.h> //Incluimos librerias
#include <Adafruit_Sensor.h> //Incluimos librerias BMP280
#include <Adafruit_BMP280.h> //Incluimos librerias BMP280

#define BMP_SCK 13 
#define BMP_MISO 12
#define BMP_MOSI 11
#define BMP_CS 10

// VCC= 5v
// GND= GND
// SCL= 21(SCL)
// SDA= 20(SDA)

Adafruit_BMP280 bmp; // I2C
//Adafruit_BMP280 bmp(BMP_CS); // hardware SPI
//Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);

void setup() {
  Serial.begin(9600);
  Serial.println(F("BMP280 test"));

  if (!bmp.begin(0x76)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }
}

void loop() {
  Serial.print(F("Temperatura = "));
  Serial.print(bmp.readTemperature());
  Serial.println(" *C");

  Serial.print(F("Presión = "));
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");

  Serial.print(F("Altitud Aproximada = "));
  Serial.print(bmp.readAltitude(1032)); // Aquí colocar la presión en hPa del lugar donde vives.
  Serial.println(" m");

  Serial.println();
  delay(2000);
}
