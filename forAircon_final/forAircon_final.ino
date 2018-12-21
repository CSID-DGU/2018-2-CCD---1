#include <SoftwareSerial.h>
#include <IRremote.h>

#include <Arduino.h>
#include <DS1302RTC.h>
#include <TimeLib.h>
DS1302RTC RTC(8,7,6);
#define DS1302_GND_PIN 5
#define DS1302_VCC_PIN 6
#include <Wire.h>
//#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#if defined(ARDUINO_ARCH_SAMD)
#define Serial SerialUSB
#endif
//RTC_Millis rtc;

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10
#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;

SoftwareSerial mySerial(4, 5); // RX, TX

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

char s_manual_value[2];
char manual_value;
char value[2];
char type;

int reyear,remonth,reday,rehour,reminute,resecond;

int IRledPin = 9;
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

  pinMode(IRledPin,OUTPUT);

  Serial.println("Goodnight moon!");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);

  temp = (unsigned long)bme.readTemperature();
  
  mySerial.write(bme.readTemperature());
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
  sscanf(Buffer,"modul %d %d %d %d %d %s %s",&modulNumber,&s_hour,&s_minute,&e_hour,&e_minute,ss_temp,&value);
  sscanf(Buffer,"c %d", &want_temp);
  sscanf(Buffer,"o %s", &s_manual_value);
  manual_value=s_manual_value[0];
  type=value[0]; 
  sss_temp=ss_temp;
  auto_temp=sss_temp.toInt();
           modul[modulNumber][1]=s_hour;
           modul[modulNumber][2]=s_minute;
           modul[modulNumber][3]=e_hour;
           modul[modulNumber][4]=e_minute;
           modul[modulNumber][5]=auto_temp;
           modul[modulNumber][6]=type;
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
//  Serial.print("date = ");
//  Serial.println(date);
  Serial.print("");
  delay(1);
  mySerial.write(bme.readTemperature());
  delay(1); 
  //printRTC(t);

    delay(1);
 // put your main code here, to run repeatedly:
  //Menual Mode
  if(manual_value =='t'){ // on요청이 들어오고 에어컨이 꺼져 있을 때turn on
    TurnOn();
 //   Serial.println("on");
//    send_wanted_temp(want_temp);
    delay(1);
    if (temp != want_temp) { //when you turn on the AC, current and want are not same
      tempUpDown(temp, want_temp);
      Serial.println("updown");
      //send_wanted_temp(want_temp);
      delay(1);
      }
    }
   
  if(manual_value == 'f'){ // when you want turn AC off, turn off
    TurnOff();
 //   Serial.println("off");
    delay(1);
    }

   //Auto Mode
 if(modulNumber>0){//모듈이 있을 때
  for(int i=1;i<=modulNumber;i++){//모듈을 하나씩 돌면서
  //  if(modul[i][6]=='t'){// 자율모드가 on일 때
      if(modul[modulNumber][1] == hour(t) && modul[modulNumber][2] == minute(t)&&second(t)==0){ //시작시간이 일치하면
        TurnOn();
        tempUpDown(temp, modul[modulNumber][5]);
        Serial.println("에어컨 자동모드 on");
        //send_wanted_temp(modul[modulNumber][5]); 
        } 
      }
  //  }

   for(int i=1;i<=modulNumber;i++){//모듈을 하나씩 돌면서
   // if(modul[i][6]=='f'){// 자동모드가 off일 때
      if(modul[modulNumber][3] == hour(t) && modul[modulNumber][4] == minute(t)&&second(t)==0){//앤딩시간이 일치하면
          TurnOff(); 
          Serial.println("에어컨 자동모드 off");
      }
    }
   }
}
//}
void printRTC(time_t t){
    Serial.print(year(t), DEC);
    Serial.print('/');
    Serial.print(month(t), DEC);
    Serial.print('/');
    Serial.print(day(t), DEC);
    Serial.print(' ');
    Serial.print(hour(t), DEC);
    Serial.print(':');
    Serial.print(minute(t), DEC);
    Serial.print(':');
    Serial.print(second(t), DEC);
    Serial.println();
}

void pulseIR(long microsecs) {
  // we'll count down from the number of microseconds we are told to wait
 
  cli();  // this turns off any background interrupts
 
  while (microsecs > 0) {
    // 38 kHz is about 13 microseconds high and 13 microseconds low
   digitalWrite(IRledPin, HIGH);  // this takes about 3 microseconds to happen
   delayMicroseconds(10);         // hang out for 10 microseconds
   digitalWrite(IRledPin, LOW);   // this also takes about 3 microseconds
   delayMicroseconds(10);         // hang out for 10 microseconds

   // so 26 microseconds altogether
   microsecs -= 26;
  }
  sei();  // this turns them back on
}

void tempUpDown(int temp1, int temp2){//temp1==current, temp2==desired
if(temp1 < temp2){ //temp up
   count = temp2 - temp1;
   for(int i=0;i<count;i++) {
    TempUp;
    delay(1); 
   }
   }
  else if(temp1 > temp2){//temp down
    count = temp1 -temp2;
    for(int i=0;i<count;i++){
      TempDown;
      delay(1);
    }
     }
 }
  
void TurnOn() {
  delayMicroseconds(31748); //Time off (Left Column on serial monitor)
  pulseIR(2940);           //Time on  (Right Column on serial monitor)
  delayMicroseconds(8880);
  pulseIR(500);
  delayMicroseconds(1520);
  pulseIR(480);
  delayMicroseconds(540);
  pulseIR(500);
  delayMicroseconds(560);
  pulseIR(460);
  delayMicroseconds(560);
  pulseIR(500);
  delayMicroseconds(560);
  pulseIR(460);
  delayMicroseconds(560);
  pulseIR(500);
  delayMicroseconds(520);
  pulseIR(480);
  delayMicroseconds(540);
  pulseIR(520);
  delayMicroseconds(560);
  pulseIR(500);
  delayMicroseconds(1500);
  pulseIR(480);
  delayMicroseconds(560);
  pulseIR(480);
  delayMicroseconds(580);
  pulseIR(460);
  delayMicroseconds(560);
  pulseIR(500);
  delayMicroseconds(540);
  pulseIR(480);
  delayMicroseconds(1580);
  pulseIR(460);
  delayMicroseconds(1580);
  pulseIR(500);
  delayMicroseconds(540);
  pulseIR(520);
  delayMicroseconds(1500);
  pulseIR(480);
  delayMicroseconds(1540);
  pulseIR(500);
  delayMicroseconds(1520);
  pulseIR(500);
  delayMicroseconds(1500);
  pulseIR(480);
  delayMicroseconds(1540);
  pulseIR(520);
  delayMicroseconds(1540);
  pulseIR(460);
  delayMicroseconds(1580);
  pulseIR(480);
  delayMicroseconds(1600);
  pulseIR(480);
  delayMicroseconds(540);
  pulseIR(520);
  delayMicroseconds(540);
  pulseIR(460);
  delayMicroseconds(580);
  pulseIR(480);
  delayMicroseconds(540);
  pulseIR(520);
  delayMicroseconds(1480);
  pulseIR(500);
  delayMicroseconds(560);
  pulseIR(460);
  delayMicroseconds(560);
  pulseIR(500);
  delayMicroseconds(1500);
  pulseIR(500);
  delayMicroseconds(1520);
  pulseIR(480);
  delayMicroseconds(540);
  pulseIR(480);
  delayMicroseconds(580);
  pulseIR(520);
  delayMicroseconds(1500);
  pulseIR(520);
  delayMicroseconds(560);
  pulseIR(500);
  delayMicroseconds(520);
  pulseIR(480);
  delayMicroseconds(1540);
  pulseIR(480);
  delayMicroseconds(1580);
  pulseIR(460);
  delayMicroseconds(560);
  pulseIR(480);
  delayMicroseconds(1540);
  pulseIR(480);
  delayMicroseconds(1540);
  pulseIR(480);
  delayMicroseconds(560);
  pulseIR(500);
  delayMicroseconds(540);
  pulseIR(440);
  delayMicroseconds(560);
  pulseIR(460);
  delayMicroseconds(580);
  pulseIR(480);
  delayMicroseconds(540);
  pulseIR(520);
  delayMicroseconds(540);
  pulseIR(440);
  delayMicroseconds(560);
  pulseIR(480);
  delayMicroseconds(600);
  pulseIR(500);
  delayMicroseconds(1500);
  pulseIR(460);
  delayMicroseconds(1560);
  pulseIR(460);
  delayMicroseconds(1560);
  pulseIR(440);
  delayMicroseconds(1560);
  pulseIR(640);
}

void TempUp() {
  // This is the code for the CHANNEL + for the downstairs TV COMCAST
  delayMicroseconds(13712); //Time off (Left Column on serial monitor)
  pulseIR(2920);           //Time on  (Right Column on serial monitor)
  delayMicroseconds(8880);
  pulseIR(520);
  delayMicroseconds(1520);
  pulseIR(480);
  delayMicroseconds(540);
  pulseIR(500);
  delayMicroseconds(520);
  pulseIR(500);
  delayMicroseconds(520);
  pulseIR(500);
  delayMicroseconds(560);
  pulseIR(460);
  delayMicroseconds(560);
  pulseIR(500);
  delayMicroseconds(540);
  pulseIR(480);
  delayMicroseconds(560);
  pulseIR(500);
  delayMicroseconds(540);
  pulseIR(480);
  delayMicroseconds(1520);
  pulseIR(500);
  delayMicroseconds(520);
  pulseIR(500);
  delayMicroseconds(520);
  pulseIR(500);
  delayMicroseconds(520);
  pulseIR(500);
  delayMicroseconds(560);
  pulseIR(500);
  delayMicroseconds(1500);
  pulseIR(540);
  delayMicroseconds(1540);
  pulseIR(540);
  delayMicroseconds(500);
  pulseIR(540);
  delayMicroseconds(1480);
  pulseIR(500);
  delayMicroseconds(1540);
  pulseIR(480);
  delayMicroseconds(1540);
  pulseIR(500);
  delayMicroseconds(1500);
  pulseIR(520);
  delayMicroseconds(1560);
  pulseIR(460);
  delayMicroseconds(1540);
  pulseIR(460);
  delayMicroseconds(1560);
  pulseIR(500);
  delayMicroseconds(1520);
  pulseIR(500);
  delayMicroseconds(520);
  pulseIR(560);
  delayMicroseconds(520);
  pulseIR(500);
  delayMicroseconds(520);
  pulseIR(480);
  delayMicroseconds(560);
  pulseIR(460);
  delayMicroseconds(1560);
  pulseIR(480);
  delayMicroseconds(540);
  pulseIR(460);
  delayMicroseconds(580);
  pulseIR(500);
  delayMicroseconds(1500);
  pulseIR(480);
  delayMicroseconds(1540);
  pulseIR(520);
  delayMicroseconds(540);
  pulseIR(460);
  delayMicroseconds(560);
  pulseIR(500);
  delayMicroseconds(540);
  pulseIR(480);
  delayMicroseconds(1520);
  pulseIR(520);
  delayMicroseconds(520);
  pulseIR(480);
  delayMicroseconds(1540);
  pulseIR(460);
  delayMicroseconds(1540);
  pulseIR(460);
  delayMicroseconds(600);
  pulseIR(480);
  delayMicroseconds(1600);
  pulseIR(520);
  delayMicroseconds(1480);
  pulseIR(480);
  delayMicroseconds(580);
  pulseIR(460);
  delayMicroseconds(560);
  pulseIR(500);
  delayMicroseconds(540);
  pulseIR(500);
  delayMicroseconds(500);
  pulseIR(500);
  delayMicroseconds(540);
  pulseIR(480);
  delayMicroseconds(560);
  pulseIR(480);
  delayMicroseconds(560);
  pulseIR(480);
  delayMicroseconds(540);
  pulseIR(500);
  delayMicroseconds(1500);
  pulseIR(480);
  delayMicroseconds(1560);
  pulseIR(460);
  delayMicroseconds(1560);
  pulseIR(500);
  delayMicroseconds(1520);
  pulseIR(440);
}

void TempDown() {
  // This is the code for the CHANNEL + for the downstairs TV COMCAST
  delayMicroseconds(56600); //Time off (Left Column on serial monitor)
  pulseIR(2940);           //Time on  (Right Column on serial monitor)
  delayMicroseconds(8900);
  pulseIR(480);
  delayMicroseconds(1560);
  pulseIR(500);
  delayMicroseconds(520);
  pulseIR(460);
  delayMicroseconds(580);
  pulseIR(460);
  delayMicroseconds(560);
  pulseIR(480);
  delayMicroseconds(540);
  pulseIR(500);
  delayMicroseconds(560);
  pulseIR(480);
  delayMicroseconds(560);
  pulseIR(480);
  delayMicroseconds(540);
  pulseIR(500);
  delayMicroseconds(560);
  pulseIR(480);
  delayMicroseconds(1540);
  pulseIR(520);
  delayMicroseconds(580);
  pulseIR(480);
  delayMicroseconds(540);
  pulseIR(480);
  delayMicroseconds(580);
  pulseIR(480);
  delayMicroseconds(560);
  pulseIR(480);
  delayMicroseconds(1580);
  pulseIR(480);
  delayMicroseconds(1580);
  pulseIR(460);
  delayMicroseconds(580);
  pulseIR(480);
  delayMicroseconds(1520);
  pulseIR(480);
  delayMicroseconds(1540);
  pulseIR(520);
  delayMicroseconds(1480);
  pulseIR(480);
  delayMicroseconds(1540);
  pulseIR(480);
  delayMicroseconds(1560);
  pulseIR(460);
  delayMicroseconds(1580);
  pulseIR(480);
  delayMicroseconds(1580);
  pulseIR(500);
  delayMicroseconds(1540);
  pulseIR(480);
  delayMicroseconds(580);
  pulseIR(460);
  delayMicroseconds(580);
  pulseIR(480);
  delayMicroseconds(580);
  pulseIR(440);
  delayMicroseconds(580);
  pulseIR(460);
  delayMicroseconds(1540);
  pulseIR(460);
  delayMicroseconds(580);
  pulseIR(440);
  delayMicroseconds(580);
  pulseIR(500);
  delayMicroseconds(1500);
  pulseIR(480);
  delayMicroseconds(1560);
  pulseIR(500);
  delayMicroseconds(520);
  pulseIR(460);
  delayMicroseconds(560);
  pulseIR(560);
  delayMicroseconds(1500);
  pulseIR(520);
  delayMicroseconds(540);
  pulseIR(480);
  delayMicroseconds(580);
  pulseIR(440);
  delayMicroseconds(1560);
  pulseIR(460);
  delayMicroseconds(1580);
  pulseIR(520);
  delayMicroseconds(500);
  pulseIR(480);
  delayMicroseconds(1540);
  pulseIR(520);
  delayMicroseconds(1500);
  pulseIR(460);
  delayMicroseconds(580);
  pulseIR(500);
  delayMicroseconds(540);
  pulseIR(480);
  delayMicroseconds(560);
  pulseIR(440);
  delayMicroseconds(540);
  pulseIR(480);
  delayMicroseconds(580);
  pulseIR(460);
  delayMicroseconds(560);
  pulseIR(500);
  delayMicroseconds(560);
  pulseIR(460);
  delayMicroseconds(580);
  pulseIR(440);
  delayMicroseconds(1560);
  pulseIR(480);
  delayMicroseconds(1540);
  pulseIR(520);
  delayMicroseconds(1520);
  pulseIR(440);
  delayMicroseconds(1540);
  pulseIR(580);
}

void TurnOff() {
  delayMicroseconds(23516); //Time off (Left Column on serial monitor)
  pulseIR(3100);           //Time on  (Right Column on serial monitor)
  delayMicroseconds(8620);
  pulseIR(520);
  delayMicroseconds(1540);
  pulseIR(500);
  delayMicroseconds(500);
  pulseIR(500);
  delayMicroseconds(500);
  pulseIR(520);
  delayMicroseconds(480);
  pulseIR(520);
  delayMicroseconds(480);
  pulseIR(520);
  delayMicroseconds(500);
  pulseIR(500);
  delayMicroseconds(500);
  pulseIR(520);
  delayMicroseconds(480);
  pulseIR(520);
  delayMicroseconds(500);
  pulseIR(520);
  delayMicroseconds(1520);
  pulseIR(520);
  delayMicroseconds(480);
  pulseIR(520);
  delayMicroseconds(500);
  pulseIR(520);
  delayMicroseconds(480);
  pulseIR(520);
  delayMicroseconds(480);
  pulseIR(520);
  delayMicroseconds(480);
  pulseIR(520);
  delayMicroseconds(480);
  pulseIR(520);
  delayMicroseconds(1520);
  pulseIR(520);
  delayMicroseconds(1520);
  pulseIR(520);
  delayMicroseconds(1520);
  pulseIR(520);
  delayMicroseconds(1520);
  pulseIR(520);
  delayMicroseconds(480);
  pulseIR(520);
  delayMicroseconds(1520);
  pulseIR(520);
  delayMicroseconds(480);
  pulseIR(520);
  delayMicroseconds(1520);
  pulseIR(520);
  delayMicroseconds(1520);
  pulseIR(520);
  delayMicroseconds(480);
  pulseIR(520);
  delayMicroseconds(500);
  pulseIR(520);
  delayMicroseconds(480);
  pulseIR(520);
  delayMicroseconds(500);
  pulseIR(520);
  delayMicroseconds(1520);
  pulseIR(520);
  delayMicroseconds(480);
  pulseIR(520);
  delayMicroseconds(520);
  pulseIR(520);
  delayMicroseconds(1520);
  pulseIR(520);
  delayMicroseconds(1520);
  pulseIR(520);
  delayMicroseconds(480);
  pulseIR(520);
  delayMicroseconds(480);
  pulseIR(520);
  delayMicroseconds(480);
  pulseIR(520);
  delayMicroseconds(1520);
  pulseIR(520);
  delayMicroseconds(1520);
  pulseIR(520);
  delayMicroseconds(1520);
  pulseIR(520);
  delayMicroseconds(1520);
  pulseIR(520);
  delayMicroseconds(480);
  pulseIR(520);
  delayMicroseconds(480);
  pulseIR(520);
  delayMicroseconds(520);
  pulseIR(520);
  delayMicroseconds(1520);
  pulseIR(500);
  delayMicroseconds(500);
  pulseIR(500);
  delayMicroseconds(500);
  pulseIR(520);
  delayMicroseconds(480);
  pulseIR(520);
  delayMicroseconds(480);
  pulseIR(520);
  delayMicroseconds(500);
  pulseIR(500);
  delayMicroseconds(500);
  pulseIR(520);
  delayMicroseconds(480);
  pulseIR(520);
  delayMicroseconds(480);
  pulseIR(520);
  delayMicroseconds(520);
  pulseIR(500);
  delayMicroseconds(1540);
  pulseIR(500);
  delayMicroseconds(1540);
  pulseIR(500);
}
