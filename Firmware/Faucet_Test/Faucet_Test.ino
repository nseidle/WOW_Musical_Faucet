/*
 This tests the raw output of an IR sensor found on a faucet that turns on hands-free.
 By: Nathan Seidle
 SparkFun Electronics
 Date: June 11th, 2015
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

 Faucet:
 Green -> A0
 Yellow -> 5V
 Black -> GND
 Red -> GND
 
 The module's internal LED should blink when hands are present.
 
*/

const byte faucetSensor = A0;

void setup()
{
  Serial.begin(9600);
  
  pinMode(faucetSensor, INPUT);
}

void loop()
{
  int sensor = analogRead(faucetSensor);
  
  Serial.print("Faucet: ");
  Serial.print(sensor);
  
  if(sensor < 900)
  {
    Serial.print(" Hands!");
  }
  
  Serial.println();
  
  delay(25);
}


