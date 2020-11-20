#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

#define SERVO_PIN 3

#define CE 9
#define CSN 10 
RF24 radio(CE , CSN); // CE, CSN
const uint64_t pipe = 0xE8E8F0F0E1LL; // định địa chỉ luồng gửi/nhận dữ liệu

//Struct Lưu dữ liệu giử đến từ mạch phát
struct Mydata{
  int valY;
  int valX;
};
Mydata data;

#define enA 5
#define in1 2
#define in2 4
#define enB 6
#define in3 7
#define in4 8
int motorSpeedA = 0;
int motorSpeedB = 0;
void setup() {
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  radio.begin();
  // Set the PA Level low to prevent power supply related issues since this is a
 // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default
  radio.setPALevel(RF24_PA_LOW); 
  radio.openReadingPipe(0, pipe);
  radio.startListening();
}

void loop() {
  //radio.startListening(); //phai khai bao lai ham nay o day, se chi nhan mot lan neu ko khai bao lai
  if(radio.available()){
    if(radio.available()){
      radio.read(&data, sizeof(Mydata));
    }
    if (data.valY < 470) {
      // Set Motor A backward
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      // Set Motor B backward
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
      
      motorSpeedA = map(data.valY, 470, 0, 0, 255);
      motorSpeedB = map(data.valY, 470, 0, 0, 255);
    }
    else if (data.valY > 550) {
      // Set Motor A forward
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      // Set Motor B forward
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
      
      motorSpeedA = map(data.valY, 550, 1023, 0, 255);
      motorSpeedB = map(data.valY, 550, 1023, 0, 255);
    }
    // If joystick stays in middle the motors are not moving
    else {
      motorSpeedA = 0;
      motorSpeedB = 0;
    } 
    if (data.valX < 470) {
      // Convert the declining X-axis readings from 470 to 0 into increasing 0 to 255 value
      int xMapped = map(data.valX, 470, 0, 0, 255);
      // Move to left - decrease left motor speed, increase right motor speed
      motorSpeedA = motorSpeedA - xMapped;
      motorSpeedB = motorSpeedB + xMapped;
      // Confine the range from 0 to 255
      if (motorSpeedA < 0) {
        motorSpeedA = 0;
      }
      if (motorSpeedB > 255) {
        motorSpeedB = 255;
      }
    }
    if (data.valX > 550) {
      // Convert the increasing X-axis readings from 550 to 1023 into 0 to 255 value
      int xMapped = map(data.valX, 550, 1023, 0, 255);
      // Move right - decrease right motor speed, increase left motor speed
      motorSpeedA = motorSpeedA + xMapped;
      motorSpeedB = motorSpeedB - xMapped;
      // Confine the range from 0 to 255
      if (motorSpeedA > 255) {
        motorSpeedA = 255;
      }
      if (motorSpeedB < 0) {
        motorSpeedB = 0;
      }
    }
    // Prevent buzzing at low speeds (Adjust according to your motors. My motors couldn't start moving if PWM value was below value of 70)
    if (motorSpeedA < 70) {
      motorSpeedA = 0;
    }
    if (motorSpeedB < 70) {
      motorSpeedB = 0;
    }

    analogWrite(enA, motorSpeedA); // Send PWM signal to motor A
    analogWrite(enB, motorSpeedB); // Send PWM signal to motor B
  }else{
    Serial.println("NO SIGNAL");
  }
}
