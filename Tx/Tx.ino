#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "mpu6050.h"

float angleX,angleY,angleZ;
#define CE 9
#define CSN 10
RF24 radio(CE , CSN); // CE, CSN
const uint64_t pipe = 0xE8E8F0F0E1LL; // định địa chỉ luồng gửi/nhận dữ liệu
#define PIN_MODE_MPU 5
#define PIN_MODE_JOY 4
#define TrucX A0
#define TrucY A2
#define LedRF 1 //bao trang thai ket noi rf
int toi,lui,trai,phai;

struct Mydata{
  int valX; 
  int valY;
};
Mydata data;

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(pipe);
  mpu6050();
  pinMode(PIN_MODE_MPU,INPUT_PULLUP);
  pinMode(PIN_MODE_JOY,INPUT_PULLUP);
  pinMode(TrucX, INPUT); //truc X joystick
  pinMode(TrucY, INPUT); //truc Y joystick
  pinMode(LedRF, OUTPUT);

  digitalWrite(LedRF,HIGH);
  delay(500);
  digitalWrite(LedRF,LOW);
  delay(500);
  digitalWrite(LedRF,HIGH);
  delay(500);
  digitalWrite(LedRF,LOW);
}

void loop() {
  if(digitalRead(PIN_MODE_MPU)==LOW){
    angleX =map(get_accx(),-16384,16384,-90,90);
    angleY =map(get_accy(),-16384,16384,-90,90);
    data.valX = map(angleX,-90,90,1023,0);
    data.valY = map(angleY,-90,90,0,1023);
  }else if(digitalRead(PIN_MODE_JOY)==LOW){
    data.valX = analogRead(TrucX); // doc truc X
    data.valY = analogRead(TrucY); // doc truc Y
  }else{
    data.valX =510;
    data.valY =510;
  }
  
  if(data.valX>550){
      toi = map(data.valX,550,1023,0,100);
      lui = 0;
    }
    else if(data.valX<470){
      toi = 0;
      lui = map(data.valX,470,0,0,100);
    }
    else{
      toi =0;
      lui =0;
    }
    if(data.valY>550){
      phai = map(data.valY,550,1023,0,100);
      trai = 0;
    }
    else if(data.valY<470){
      phai = 0;
      trai = map(data.valY,470,0,0,100);
    }
    else{
      phai =0;
      trai =0;
    }
    String senddata= String(toi)+","+String(lui)+","+String(phai)+","+String(trai);
    Serial.println(senddata);
  //Serial.print(data.valX); Serial.print("\t");Serial.println(data.valY);
  radio.write(&data,sizeof(Mydata));
  
}
