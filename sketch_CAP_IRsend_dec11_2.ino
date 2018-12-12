#include <IRremote.h>

IRsend irsend;
int khz = 38;

void setup() {
  //for sending IR
  Serial.begin(9600);
  

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("on");
  turnOn();
  delay(10000);

  Serial.println("up");
  tempUp();
  delay(10000);


  Serial.println("down");
  tempDown();
  delay(10000);

  Serial.println("off");
  turnOff();
  delay(10000);

}

void turnOn(){
  //remote 1 FF30CF
  unsigned int turnon[] = {9100,4400, 600,550, 650,500, 650,550, 600,550, 600,550, 600,550, 600,550, 600,550, 600,1600, 600,1600, 650,1600, 600,1600, 650,1550, 650,1600, 650,1550, 650,1600, 650,500, 650,500, 650,1550, 650,1600, 650,500, 650,500, 650,500, 650,550, 600,1550, 650,1600, 650,500, 650,500, 650,1550, 650,1600, 600,1600, 650,1600, 600};  // NEC FF30CF
  irsend.sendRaw (turnon, sizeof(turnon) / sizeof(turnon[0]) , khz); // Note the apporach used to automatically calcualate the size of the array;
 
 }

void turnOff(){
  //remote 0 FF42BD
   unsigned int turnoff[] = {9000,4450, 600,550, 600,550, 600,550, 600,550, 600,550, 600,550, 550,600, 600,550, 600,1650, 550,1650, 600,1650, 550,1650, 600,1650, 550,1650, 600,1650, 550,1650, 600,550, 600,1650, 550,600, 550,600, 550,600, 550,600, 600,1600, 600,550, 600,1650, 550,600, 550,1650, 600,1650, 550,1650, 600,1650, 550,600, 550,1650, 600};  // NEC FF42BD
  irsend.sendRaw (turnoff, sizeof(turnoff) / sizeof(turnoff[0]) , khz); // Note the apporach used to automatically calcualate the size of the array;
  
 }

 void tempUp(){
 //remote 2 FF18E7
 unsigned int tempup[] = {9050,4400, 650,550, 600,550, 600,550, 600,550, 600,550, 600,550, 600,550, 600,550, 600,1600, 600,1600, 650,1600, 600,1600, 650,1550, 650,1600, 650,1550, 650,1600, 600,550, 600,550, 600,550, 600,1600, 650,1600, 600,550, 600,550, 600,550, 600,1600, 650,1600, 600,1600, 650,550, 600,550, 600,1600, 600,1600, 600,1600, 600};  // NEC FF18E7
 irsend.sendRaw (tempup, sizeof(tempup) / sizeof(tempup[0]) , khz); // Note the apporach used to automatically calcualate the size of the array;
 
 }
 
void tempDown(){
  //remote 3 FF7A85 
  unsigned int tempdown[] = {9050,4400, 650,550, 600,500, 650,500, 650,500, 650,500, 650,500, 650,500, 650,500, 650,1600, 600,1600, 650,1600, 600,1600, 650,1550, 650,1600, 650,1550, 650,1600, 600,550, 600,1600, 650,1600, 600,1600, 650,1600, 600,550, 600,1600, 650,500, 650,1600, 600,550, 550,600, 600,550, 600,550, 600,1600, 650,500, 650,1600, 600};  // NEC FF7A85
  irsend.sendRaw (tempdown, sizeof(tempdown) / sizeof(tempdown[0]) , khz); // Note the apporach used to automatically calcualate the size of the array;
  
 }
