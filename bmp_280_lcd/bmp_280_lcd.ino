/***************************************************************************
liabray untuk BMP280 humidity, temperature (pressure sensor)
 
Oleh Syahroni S.T
***************************************************************************/
 
#include <Wire.h>
//#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>  
 
// digunakan hanya jika menggunakan SPI untuk berkomunikasi dengan sensor
 
//#define BMP_SCK 13
//#define BMP_MISO 12
//#define BMP_MOSI 11
//#define BMP_CS 10
 
// disini menggunakan IIC
 
Adafruit_BMP280 bmp; // I2C
//Adafruit_BMP280 bmp(BMP_CS); // hardware SPI
//Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO, BMP_SCK);
 
void setup() {
Serial.begin(9600);
Serial.println(F("BMP280 test"));
 
if (!bmp.begin()) {
Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
while (1);
}
}
 
void loop() {

//untuk temperatur

//Serial.print(F("Temperature = "));
//Serial.print(bmp.readTemperature());
//Serial.println(" *C");

//untuk tekanan
 
Serial.print(F("Pressure = "));
Serial.print(bmp.readPressure());
Serial.println(" Pa");

//untuk ketinggigian
 
Serial.print(F("Approx altitude = "));
Serial.print(bmp.readAltitude(1013.25)); // untuk kalibrasi MDPL
Serial.println(" m");
 
Serial.println();
delay(2000);
}
