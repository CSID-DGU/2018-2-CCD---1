#include <SoftwareSerial.h>
#include <IRremote.h>

IRsend irsend;
int khz = 38;

#include <Arduino.h>
#include <DS1302RTC.h>
#include <TimeLib.h>
DS1302RTC RTC(8,7,6);
#define DS1302_GND_PIN 5
#define DS1302_VCC_PIN 6
#include <Wire.h>

#include "SparkFunBME280.h"
BME280 bme;


#if defined(ARDUINO_ARCH_SAMD)
#define Serial SerialUSB
#endif
//RTC_Millis rtc;

SoftwareSerial mySerial(4, 5); // RX, TX

unsigned long delayTime;
unsigned long temp;

char Buffer[100];
char date[50];
//int modulNumber;
//int s_hour;
//int s_minute;
//int e_hour;
//int e_minute;
//char ss_temp[4];
//String sss_temp;
//int auto_temp;
//int modul[8][6];
int want_temp;

char s_manual_value[2];
char manual_value;
//char value[2];
//char type;

int reyear,remonth,reday,rehour,reminute,resecond;
int count;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  bool status;
   status = bme.begin();  //SDA:A4, SCL:A5
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
    }
  delayTime = 1;

  Serial.println("Goodnight moon!");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);

  temp = (int)bme.readTempC();
  
  mySerial.write(bme.readTempC());
   digitalWrite(DS1302_GND_PIN, LOW);
  pinMode(DS1302_GND_PIN, OUTPUT);

  digitalWrite(DS1302_VCC_PIN, HIGH);
  pinMode(DS1302_VCC_PIN, OUTPUT);

  setSyncProvider(RTC.get);
   Serial.println("RTC Sync");
  if (timeStatus() == timeSet)
    Serial.println(" Ok!");
  else
    Serial.println(" FAIL!");
}

void loop() { // run over and over
    static time_t tLast;
    time_t t;
    tmElements_t tm;

      t = now();
    if (t != tLast) {
        tLast = t;
    }
    delay(1);
    
  int cnt = 0;
  //데이터 수신 여부 확인
  if (mySerial.available() < 1) {
    return 0;
  }
  //timeout 동안 1줄 데이터 읽기

  unsigned long start = millis();
  while (millis() - start < 100) {
    if (mySerial.available() < 1) {
      continue;
    }
    char data = mySerial.read();
    if (data == '\n') {
      Buffer[cnt] = '\0';
      break;
    }
    Buffer[cnt++] = data;
  }
  //잔류 데이터 지우기
  while (mySerial.available() > 0) {
    mySerial.read();
  }
  sscanf(Buffer,"DATE %s",date);
  sscanf(Buffer,"DATE %d, %d, %d, %d, %d, %d",&reyear,&remonth,&reday,&rehour,&reminute,&resecond);
//  sscanf(Buffer,"modul %d %d %d %d %d %s %s",&modulNumber,&s_hour,&s_minute,&e_hour,&e_minute,ss_temp,&value);
  sscanf(Buffer,"c %d", &want_temp);
  sscanf(Buffer,"o %s", &s_manual_value);
  manual_value=s_manual_value[0];
//  type=value[0]; 
//  sss_temp=ss_temp;
//  auto_temp=sss_temp.toInt();
//           modul[modulNumber][1]=s_hour;
//           modul[modulNumber][2]=s_minute;
//           modul[modulNumber][3]=e_hour;
//           modul[modulNumber][4]=e_minute;
//           modul[modulNumber][5]=auto_temp;
//           modul[modulNumber][6]=type;
            Serial.print("manual_on/off: ");
            Serial.println(manual_value);
            Serial.print("temp : ");
            Serial.println(temp);
            Serial.print("want_temp : ");
            Serial.println(want_temp);
//            Serial.print("modulNumber : ");
//            Serial.println(modulNumber);
//            Serial.print("start_hour : ");
//            Serial.println(modul[modulNumber][1]);
//            Serial.print("start_minute : ");
//            Serial.println(modul[modulNumber][2]);
//            Serial.print("end_hour : ");
//            Serial.println(modul[modulNumber][3]);
//            Serial.print("end_minute : ");
//            Serial.println(modul[modulNumber][4]);
//            Serial.print("auto_temp : ");
//            Serial.println(modul[modulNumber][5]);
//            Serial.print("type : ");
//            Serial.println(modul[modulNumber][6]);
//  Serial.print("date = ");
//  Serial.println(date);
  Serial.print("");
  delay(1);
  mySerial.write(bme.readTempC());
  delay(1); 
//  printRTC(t);
  

    delay(1);
 // put your main code here, to run repeatedly:
  //Menual Mode
  if(manual_value =='t'){ // on요청이 들어오고 에어컨이 꺼져 있을 때turn on
    Serial.println("on");
    TurnOn();
    delay(1);
    if (temp != want_temp) { //when you turn on the AC, current and want are not same
      Serial.println("updown"); 
      send_wanted_temp(want_temp);
      delay(20);
      Temp(); 
      delay(1);
      }
    }
   
  if(manual_value == 'f'){ // when you want turn AC off, turn off
    TurnOff();
 //   Serial.println("off");
    delay(1);
    }

}

void TurnOn(){
  //remote 9 FF52AD
  unsigned int turnon[] = {9050,4450, 550,600, 600,550, 600,550, 600,550, 600,550, 600,550, 600,550, 600,550, 600,1650, 600,1600, 600,1650, 550,1650, 550,1650, 600,1650, 550,1650, 550,1700, 550,600, 550,1650, 550,600, 550,1700, 550,600, 550,600, 550,1650, 550,600, 550,1700, 550,600, 550,1650, 550,600, 550,1700, 550,1650, 550,600, 550,1700, 550};  // NEC FF52AD
  irsend.sendRaw (turnon, sizeof(turnon) / sizeof(turnon[0]) , khz); // Note the apporach used to automatically calcualate the size of the array;
  delay(10);
}

void TurnOff(){
  //remote 6 5AA5
   unsigned int turnoff[] = {8950,4350, 650,550, 600,550, 600,500, 600,550, 600,550, 600,550, 600,550, 600,500, 600,1600, 600,1600, 600,1600, 600,1600, 600,1600, 650,1550, 600,1600, 600,1600, 650,550, 550,1600, 600,550, 600,1600, 600,1600, 600,550, 600,1600, 600,550, 600,1600, 550,600, 550,1650, 600,550, 600,550, 600,1550, 600,550, 600,1600, 600};  // NEC FF5AA5  
  irsend.sendRaw (turnoff, sizeof(turnoff) / sizeof(turnoff[0]) , khz); // Note the apporach used to automatically calcualate the size of the array;
  delay(10);
}

 void Temp(){
 //remote 2 FF18E7
 unsigned int temp[] = {9050,4400, 650,550, 600,550, 600,550, 600,550, 600,550, 600,550, 600,550, 600,550, 600,1600, 600,1600, 650,1600, 600,1600, 650,1550, 650,1600, 650,1550, 650,1600, 600,550, 600,550, 600,550, 600,1600, 650,1600, 600,550, 600,550, 600,550, 600,1600, 650,1600, 600,1600, 650,550, 600,550, 600,1600, 600,1600, 600,1600, 600};  // NEC FF18E7
 irsend.sendRaw (temp, sizeof(temp) / sizeof(temp[0]) , khz); // Note the apporach used to automatically calcualate the size of the array;
 delay(10);
 }

void Frequency(){
  //Ir remote 1 0xFF30CF;
  unsigned int frequency[] = {9100,4400, 600,550, 650,500, 650,550, 600,550, 600,550, 600,550, 600,550, 600,550, 600,1600, 600,1600, 650,1600, 600,1600, 650,1550, 650,1600, 650,1550, 650,1600, 650,500, 650,500, 650,1550, 650,1600, 650,500, 650,500, 650,500, 650,550, 600,1550, 650,1600, 650,500, 650,500, 650,1550, 650,1600, 600,1600, 650,1600, 600};  // NEC FF30CF 
  irsend.sendRaw (frequency, sizeof(frequency) / sizeof(frequency[0]) , khz); // Note the apporach used to automatically calcualate the size of the array; 
  delay(500); 
 }

 void Frequency2(){
    //unsigned int  data = 0xFF4AB5; //8번
  unsigned int frequency2[] = {8950,4400, 600,550, 600,550, 550,600, 550,550, 600,550, 600,550, 600,550, 600,550, 600,1600, 600,1600, 600,1600, 600,1600, 600,1600, 600,1650, 550,1650, 550,1650, 550,600, 550,1650, 600,550, 550,600, 550,1650, 550,600, 550,1650, 550,550, 600,1650, 550,550, 600,1650, 550,1650, 550,600, 550,1650, 550,600, 550,1650, 550};  // NEC FF4AB5
  irsend.sendRaw (frequency2, sizeof(frequency2) / sizeof(frequency2[0]) , khz); // Note the apporach used to automatically calcualate the size of the array; 
  delay(500); 
    
}
 
void send_wanted_temp(int want_temp){
  //The lowest degree is 18Cand The higest degree is 25C
  int frequency = want_temp - 17;
  Serial.println(frequency);
  if(frequency<5){
  for(int i=0;i<frequency;i++){
    delay(20);
    Frequency();
    Serial.print("f");
    delay(50);
    }
  }
  else{
   for(int i=0;i<frequency-4;i++){
    delay(20);
    Frequency2();
    Serial.print("f");
    delay(50);
   }
  }
}
   
