#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

int axisdata[5];

int upSwitch = A5;
int downSwitch = A3;
int leftSwitch = 2;
int rightSwitch = 3;
int trigger = 7;

RF24 radio(9, 10); // CE, CSN
const byte address[6] = "00001";

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600); //try 115200
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  //Serial.println("TEST nRF24L01+ transmitter");
  //radio.setChannel(76);

  
pinMode(2, INPUT_PULLUP);
 pinMode(A3, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);
  pinMode(7, INPUT);


}

void loop() {
  // put your main code here, to run repeatedly:

/*
 const char text[] = "Hello World";
  radio.write(&text, sizeof(text));
  delay(1000);
*/

int upVal = digitalRead(upSwitch);
int downVal = digitalRead(downSwitch);
int leftVal = digitalRead(leftSwitch);
int rightVal = digitalRead(rightSwitch);
int guntrig = digitalRead(trigger);


axisdata[0] = upVal;
axisdata[1] = downVal;
axisdata[2] = leftVal;
axisdata[3] = rightVal;
axisdata[4] = guntrig;

Serial.print("Up switch: ");
  Serial.println(upVal);
  Serial.print("Down switch: ");
  Serial.println(downVal);
  Serial.print("Left switch: ");
  Serial.println(leftVal);
  Serial.print("Right switch: ");
  Serial.println(rightVal);

radio.write(&axisdata, sizeof(axisdata));

}
