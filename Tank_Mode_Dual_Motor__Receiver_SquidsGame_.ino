#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//Motor 1
int enA = 9;

int in1 = 8;

int in2 = 7;

// Motor B

int enB = 6;

int in3 = 5;

int in4 = 4;

int trig = A5;

int axisdata[5];

RF24 radio(2, 3); // CE, CSN
const byte address[6] = "00001";




void setup(){
 Serial.begin(9600);
 radio.begin();
 //Serial.println("TEST nRF24L01+ receiver");
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  radio.setChannel(76);

// Set all the motor control pins to outputs

pinMode(enA, OUTPUT);

pinMode(in1, OUTPUT);

pinMode(in2, OUTPUT);

pinMode(enB, OUTPUT);

pinMode(in3, OUTPUT);

pinMode(in4, OUTPUT);

pinMode(trig, OUTPUT);


digitalWrite(in1, LOW);
digitalWrite(in2, LOW);
digitalWrite(in3, LOW);
digitalWrite(in4, LOW);

digitalWrite(trig, LOW);

analogWrite(enA, 0);
analogWrite(enB, 0);
delay(3000); 
}



void loop(){

 /* for (int i=0; i < sizeof(axisdata); i++) {
    axisdata[i] = HIGH;
  }*/
  if (radio.available()){
    radio.read(&axisdata, sizeof(axisdata));
  }
  Serial.print(axisdata[0]);
  Serial.print(axisdata[1]);
  Serial.print(axisdata[2]);
  Serial.println(axisdata[3]);
 if (axisdata[0] == HIGH && axisdata[1] == HIGH){
     // Turn on motor A
    
    digitalWrite(in1, LOW);
    
    digitalWrite(in2, LOW);
    
    // Set speed to 200 out of possible range 0~255
    
    
    analogWrite(enA, 0);
 }else if (axisdata[0] == LOW){
    // Turn on motor A
    
    digitalWrite(in1, HIGH);
    
    digitalWrite(in2, LOW);
    
    // Set speed to 200 out of possible range 0~255
    
    
    analogWrite(enA, 125);
  } else if(axisdata[1] == LOW){
        // Turn on motor A
    
    digitalWrite(in1, LOW);
    
    digitalWrite(in2, HIGH);
    
    // Set speed to 200 out of possible range 0~255
    
    analogWrite(enA, 125);
  } 
 if(axisdata[2] == HIGH && axisdata[3] == HIGH){
       // Turn on motor B
    
    digitalWrite(in3, LOW);
    
    digitalWrite(in4, LOW);
    
    // Set speed to 200 out of possible range 0~255
    
    
    analogWrite(enB, 0);
 }else if(axisdata[2] == LOW){
    // Turn on motor A

    digitalWrite(in4, LOW);
    
    digitalWrite(in3, HIGH);
    
    // Set speed to 200 out of possible range 0~255
    
    analogWrite(enB, 125);
  }else if(axisdata[3] == LOW){
      // Turn on motor A
    
    digitalWrite(in3, LOW);
    
    digitalWrite(in4, HIGH);
    
    // Set speed to 200 out of possible range 0~255
    
    analogWrite(enB, 125);
 
  }

  if (axisdata[4] == HIGH){
    digitalWrite(trig,HIGH);
  }else {
    digitalWrite(trig, LOW);
  }

}



  //delay(20);

  
