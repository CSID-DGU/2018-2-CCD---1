#include <SoftwareSerial.h>

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10
#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;

SoftwareSerial mySerial(3, 2); // RX, TX

unsigned long delayTime;
unsigned long temp;

char buffer[20];               //통신을 할때 buffer배열에 전송받은 데이터 입력
char bufferIndex = 0; 
long want_temp;
char c;
String rxString = "";
boolean bRxStart = false;
String s_modulNumber;
String start_time;
String end_time;
String s_temp;
String modul[9][5];
int tempValue;
int modulNumber;


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
  int BTtemp;
  
}

void loop() { // run over and over

  while(mySerial.available()) {
    c=mySerial.read();
    if(bRxStart == false){
      if(c=="(")bRxStart=true;//시작문자검사->문자열 저장 시작
      else{
        if(c==')'){ //종료문자 검사
           //dataParsing(rxString);//데이터 분석 후 초기화
           s_modulNumber=rxString.substring(1,rxString.indexOf('s'));
           start_time = rxString.substring(rxString.indexOf('s')+1,rxString.indexOf('e') );
           end_time = rxString.substring(rxString.indexOf('e')+1,rxString.indexOf('w'));
           s_temp =rxString.substring(rxString.indexOf('w')+1,rxString.length());
           tempValue = s_temp.toInt(); //문자열 ->int형 변환
           modulNumber=s_modulNumber.toInt();
           bRxStart = false;
           rxString="";
            modul[modulNumber][0]=start_time;
            modul[modulNumber][1]=end_time;
            modul[modulNumber][2]=s_temp;
            modul[modulNumber][3]="o";
            
            Serial.print("moduleNumber : ");
            Serial.println(modulNumber);
            Serial.print("start_time : ");
            Serial.println(modul[modulNumber][0]);
            Serial.print(" end_time : ");
            Serial.println(modul[modulNumber][1]);
            Serial.print("temp : ");
            Serial.println(modul[modulNumber][2]);
            Serial.print("type : ");
            Serial.println(modul[modulNumber][3]);
        }else{
          rxString +=c;
        }
      }
    }
  }
  
   
   
    mySerial.write(bme.readTemperature());
    delay(2000);
  
}
//static void dataParsing(String str){
//  static String modulNumber=str.substring(0, str.indexOf('s'));
//  static String start_time = str.substring(str.indexOf('s'),str.indexOf('e') );
//  static String end_time = str.substring(str.indexOf('e'),str.indexOf('w'));
//  static String temp = str.substring(str.indexOf('w'),str.indexOf());
//  static int tempValue = temp.toInt(); //문자열 ->int형 변환
//  
//}
