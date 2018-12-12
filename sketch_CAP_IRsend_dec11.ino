#include <IRremote.h>

IRsend irsend;

void setup() {
  // put your setup code here, to run once:



}

void loop() {
  // put your main code here, to run repeatedly:
  int khz = 38;

  unsigned int turnOn[] = {8950,4500, 550,600, 550,600, 550,600, 550,600, 550,600, 550,600, 550,600, 550,600, 550,1700, 550,1650, 550,1650, 600,1650, 600,1600, 550,1700, 550,1650, 550,1650, 600,600, 550,1650, 550,1650, 600,550, 600,1650, 550,600, 550,600, 550,600, 550,1650, 550,600, 600,550, 600,1650, 550,600, 550,1650, 550,1700, 550,1650, 550};  // NEC FF6897 

  irsend.sendRaw (turnOn, sizeof(turnOn) / sizeof(turnOn[0]) , khz); // Note the apporach used to automatically calcualate the size of the array;

  delay(5000);

}
