#include <SoftwareSerial.h>

#include <Arduino.h>
#include "RTClib.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#if defined(ARDUINO_ARCH_SAMD)
#define Serial SerialUSB
#endif
RTC_Millis rtc;

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10
#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;

SoftwareSerial mySerial(3, 2); // RX, TX

unsigned long delayTime;
unsigned long temp;

char Buffer[100];
char date[50];
int modulNumber;
int s_hour;
int s_minute;
int e_hour;
int e_minute;
char ss_temp[4];
String sss_temp;
int auto_temp;
int modul[8][6];
int want_temp;
int value;
int manual_value;

int year,month,day,hour,minute,second;

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
  delayTime = 1000;


  Serial.println("Goodnight moon!");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);

  temp = (unsigned long)bme.readTemperature();
  
  mySerial.write(bme.readTemperature());
  
}

void loop() { // run over and over

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
  sscanf(Buffer,"DATE %d, %d, %d, %d, %d, %d",&year,&month,&day,&hour,&minute,&second);
  sscanf(Buffer,"modul %d %d %d %d %d %s %d",&modulNumber,&s_hour,&s_minute,&e_hour,&e_minute,ss_temp,&value);
  sscanf(Buffer,"c %d", &want_temp);
  sscanf(Buffer,"o %d", &manual_value);
  rtc.adjust(DateTime(year, month, day, hour, minute, second));
  sss_temp=ss_temp;
  auto_temp=sss_temp.toInt();
           modul[modulNumber][1]=s_hour;
           modul[modulNumber][2]=s_minute;
           modul[modulNumber][3]=e_hour;
           modul[modulNumber][4]=e_minute;
           modul[modulNumber][5]=auto_temp;
           modul[modulNumber][6]=value;
            Serial.print("manual_on/off: ");
            Serial.println(manual_value);
            Serial.print("temp : ");
            Serial.println(temp);
            Serial.print("want_temp : ");
            Serial.println(want_temp);
            Serial.print("modulNumber : ");
            Serial.println(modulNumber);
            Serial.print("start_hour : ");
            Serial.println(modul[modulNumber][1]);
            Serial.print("start_minute : ");
            Serial.println(modul[modulNumber][2]);
            Serial.print("end_hour : ");
            Serial.println(modul[modulNumber][3]);
            Serial.print("end_minute : ");
            Serial.println(modul[modulNumber][4]);
            Serial.print("auto_temp : ");
            Serial.println(modul[modulNumber][5]);
            Serial.print("type : ");
            Serial.println(modul[modulNumber][6]);
  Serial.print("date = ");
  Serial.println(date);
  Serial.print("");
  delay(3000);
  mySerial.write(bme.readTemperature());
  delay(5000);
  DateTime now = rtc.now();
  Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    delay(3000);
}

  
