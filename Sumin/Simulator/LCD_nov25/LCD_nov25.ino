//I2C library
#include<Wire.h>
//I2C LCD Library
#include<LiquidCrystal_I2C.h>

//LCD I2C address
LiquidCrystal_I2C lcd(0x27,16,2);
int Temp;

void setup()
{
  // initialize the LCD
  lcd.init(); // lcd를 사용을 시작합니다.
  lcd.backlight(); // backlight를 On 시킵니다.
  lcd.print("Hello, world!"); // 화면에 Hello, world!를 출력합니다.
  delay(3000);
}

void loop(){

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Current: ");
  lcd.setCursor(10,0);
  lcd.print("25");
  delay(5000);
  
  lcd.setCursor(0,1);
  lcd.print("Power ON");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("Power OFF");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("Temp UP");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("Temp Down");
  delay(2000);

 
}

