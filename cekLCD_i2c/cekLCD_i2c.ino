#include <Wire.h> // memasukan library komunikasi I2C
#include <LiquidCrystal_I2C.h> //memasukan library LCD I2C

LiquidCrystal_I2C lcd(0x27 ,16,2); 
//Ubah alamat 0x27 dengan alamat i2C kamu
// untuk lcd 20x4 bisa ganti 16, 2 kode diatas menjadi 20,4
void setup()
  {
    lcd.backlight();
    lcd.begin (); 
  }
void loop()
{  
  lcd.setCursor(0, 0); //baris pertama   
  lcd.print("TEST LCD i2C");    
  lcd.setCursor(0, 1); //baris kedua   
  lcd.print(" Cronyos.com ");
}
