#include <IRremote.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>

#include "SparkFunBME280.h"

int RECV_PIN = 5;
IRrecv irrecv(RECV_PIN);
decode_results results;

//Temp
int current;
int desired;

//LED
int led = 4;

//DC MOTOR
#define MOTORA_1 6
#define MOTORA_2 7
#define MOTORB_1 9
#define MOTORB_2 8
int velocity;

//LCD I2C address
LiquidCrystal_I2C lcd(0x27,16,2);

//BME 
BME280 mySensor;

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver

  pinMode(led,OUTPUT);
  
  pinMode(MOTORA_1, OUTPUT);
  pinMode(MOTORA_2, OUTPUT);
  pinMode(MOTORB_1, OUTPUT);
  pinMode(MOTORB_2, OUTPUT);

  lcd.init(); 
  
  lcd.clear();
  lcd.backlight(); 

  if (mySensor.beginSPI(10) == false) //Begin communication over SPI. Use pin 10 as CS.
  {
    Serial.println("The sensor did not respond. Please check wiring.");
    while(1); //Freeze
  }
}

//frequency
int frequency = 0;

void loop() {
  if (irrecv.decode(&results)) {
    int a = results.value;
    Serial.println(a, HEX);

    //희망온도 받기
    //6
   if(a == 0x30CF){ //프리퀀시 한 번받을 때 희망온도 1도씩 증가
      frequency ++;
      }
     //8
    
    if(frequency>8) //프리퀀시가 25도 넘을시
    frequency = 1;// 희망온도 다시 1로
   
    switch(frequency){
      case 1: 
        desired =18; 
        break;
      case 2: 
        desired = 19;
        break;
      case 3:
        desired = 20;
        break;
      case 4:
        desired = 21;
        break;  
      case 5: 
        desired = 22; 
        break;
      case 6: 
        desired = 23;
        break;
      case 7:
        desired = 24;
        break;
      case 8:
        desired = 25;
        break;        
      }
    if(desired >0){ //off일 때 desired가 -1이므로 lcd에 입력되지 않음.
      current = mySensor.readTempC();
      Serial.print(mySensor.readTempC(), 2);
 
      lcd.setCursor(10,0);
      lcd.print(current);

      lcd.setCursor(10,1);
      lcd.print(desired);       
    }
    
    if(a == 0x52AD) { //turn on
      Serial.println("on");
      digitalWrite(led,HIGH);
      Serial.println("led");
      
      lcd.backlight(); 

      current = mySensor.readTempC();
      Serial.print(mySensor.readTempC(), 2);
      lcd.setCursor(0,0);
      lcd.print("Current: ");
      lcd.setCursor(10,0);
      lcd.print(current);

      lcd.setCursor(0,1);
      lcd.print("Desired: ");
      lcd.setCursor(10,1);
      lcd.print(desired);  
   }

    if(desired >0){ //off일 때 desired가 -1이므로 lcd에 입력되지 않음.
      current = mySensor.readTempC();
      Serial.print(mySensor.readTempC(), 2);
      lcd.setCursor(10,0);
      lcd.print(current);

      lcd.setCursor(10,1);
      lcd.print(desired);       
    }
    
    
   if(a == 0x18E7) { //temp up
     velocity = 45 - (desired - 18) * 2;
     Serial.println("up");
     analogWrite(MOTORA_1,velocity);
     digitalWrite(MOTORA_2,LOW);
     analogWrite(MOTORB_1,velocity);
     digitalWrite(MOTORB_2,LOW);

     lcd.setCursor(10,1);
     lcd.print(desired);
   }

   if(a == 0x5AA5) { //turn off
     Serial.println("off");
     digitalWrite(led,LOW);     
     digitalWrite(MOTORA_1,LOW);
     digitalWrite(MOTORA_2,LOW);
     digitalWrite(MOTORB_1,LOW);
     digitalWrite(MOTORB_2,LOW);
     //lcd 소등
     lcd.clear();
     lcd.noBacklight();
     //희망온도 초기화
     desired = -1;
     frequency = 0;
   }
   irrecv.resume(); // Receive the next value 
  }
  delay(100);
}
