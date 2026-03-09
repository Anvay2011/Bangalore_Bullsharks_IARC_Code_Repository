#include <Arduino.h>
#define lf 14
#define lb 27
#define rf 13
#define rb 12
#define NUM_SENSORS 8
int distance;
int L4 = 26;
int L3= 25;
int L2= 33;
int L1 = 32;
int R1 = 35;
int R2 = 34;
int R3=15;
int R4=2;
int l4;
int l3;
int l2;
int l1;
int r1;
int r2;
int r3;
int r4;
int sensorPins[NUM_SENSORS] = {L4, L3, L2, L1, R1, R2, R3, R4};
int sensorValues[NUM_SENSORS];
void setup() {
  Serial.begin(9600);
  for (int i = 0; i < NUM_SENSORS; i++) {
        pinMode(sensorPins[i], INPUT);
    }
    // **Set motor driver pins as OUTPUT**
    pinMode(lf, OUTPUT);
    pinMode(lb, OUTPUT);
    pinMode(rf, OUTPUT);
    pinMode(rb, OUTPUT);
}

void loop() {
  int r4 =  digitalRead(R4);
  int r3 =  digitalRead(R3);
  int r2 =  digitalRead(R2);
  int r1 =  digitalRead(R1);
  int l4 =  digitalRead(L4);
  int l3 =  digitalRead(L3);
  int l2 =  digitalRead(L2);
  int l1 =  digitalRead(L1);
   if(r4==0 ){
    Serial.println("Pin: r4");
  }
  else if(r3==0 ){
    Serial.println("Pin: r3");
  }
  else if(r2==0 ){
    Serial.println("Pin: r2");
  }
  else if((r1)==0 ){
    Serial.println("Pin: r1");
  }
    else if(l1==0 ){
    Serial.println("Pin: l1");
  }
  else if(l2==0 ){
    Serial.println("Pin: l2");
  }
  else if(l3==0 ){
    Serial.println("Pin: l3");
  }
  else if(l4==0 ){
    Serial.println("Pin: l4");
  }
   else{
    Serial.println("Nothing");
  }
}
  



