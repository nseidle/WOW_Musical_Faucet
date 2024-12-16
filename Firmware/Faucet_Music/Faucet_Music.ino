/*
 This tests the raw output of an IR sensor found on a faucet that turns on hands-free.
 By: Nathan Seidle
 SparkFun Electronics
 Date: June 11th, 2015
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

 We read the sensor from an off the shelf 'hands free faucet' and playing music each time we detect hands.

 Runs on SparkFun RedBoard Plus: https://www.sparkfun.com/products/18158
 and the MP3 Shield: https://www.sparkfun.com/products/12660

 Faucet:
 Green -> A0
 Yellow -> D5 - It was 5V then we needed to power cycle/reset the faucet
 Black -> GND
 Red -> GND

 Audio Board
 OUT+/- -> Speaker
 IN+/- -> MP3 Shield R/-
 +/- -> 5V/GND
 S -> A1
 Trimpot for Volume
 BrownBlackOrange resistor on VOL (jumper open)

 Does the audio sound like a monster? Covert the WAV or MP3 to 64kHz.

 Good online editor: https://twistedwave.com/online/ (cut, amplify, VBR save)
 Audio Cutter: http://mp3cut.net/
 MP3 Volume Increase: http://www.mp3louder.com/
 Down sample MP3s to 64kHz: http://online-audio-converter.com/
*/

//Include various libraries
#include <SdFat.h> //SDFat Library - http://librarymanager/All#sdfat_exfat by Bill Greiman

#include <vs1053_SdFat.h> //Mp3 Shield Library - http://librarymanager/All#VS1053_sdFat by Michael Flaga
vs1053 MP3player; //Create MP3 library object
SdFat sd; //Create object to handle SD functions

#include <avr/wdt.h> //We need watch dog for this program

//Hardware definitions
const byte faucetSensor = A0;
const byte ampEnable = A1;
const byte faucetPower = 5;

boolean playerStopped = false; //These are booleans used to control the main loop

long lastMillis = 0; //Keeps track of how long since last button press
byte previousTrack1 = 1; //Used to prevent the playing of sounds twice in a row
byte previousTrack2 = 2;

void setup()
{
  wdt_reset(); //Pet the dog
  wdt_disable(); //We don't want the watchdog during init

  Serial.begin(115200);
  
  randomSeed(analogRead(A2)); //Feed the ether into the random number generator
  
  pinMode(faucetSensor, INPUT);
  pinMode(ampEnable, OUTPUT);
  pinMode(faucetPower, OUTPUT);
  
  digitalWrite(ampEnable, HIGH);
  digitalWrite(faucetPower, LOW); //Start with faucet on

  initSD(); //Initialize the SD card
  initMP3Player(); // Initialize the MP3 Shield

  playTrack("bell.mp3");

  Serial.println("Faucet music player online");

  wdt_reset(); //Pet the dog
  //wdt_enable(WDTO_500MS); //Unleash the beast
  //It looks like when we play a wav file the processor spends 100%
  //of its time decoding so we never get the chance to pet the dog
  //Mp3 decoding seems to drop to the main loop just fine
}

void loop()
{
  wdt_reset(); //Pet the dog

  int sensor = analogRead(faucetSensor);
  
  Serial.print("Faucet: ");
  Serial.println(sensor);
  
  if(playerStopped == true && sensor < 900)
  {
    Serial.println("Hands!");
    
    if(MP3player.isPlaying() == false)
    {
      //playTrack("bell.mp3"); //Used during testing
      playRandomTrack(); //Play a random track
    }
    else
    {
      Serial.println("Still playing previous track");
    }
  }
  
  //Turn off MP3 chip when not playing
  if (MP3player.isPlaying() == false && playerStopped == false)
  {
    wdt_reset(); //Pet the dog
    Serial.println("Shutting down MP3, powering up faucet sensor");
    playerStopped = true;

    digitalWrite(faucetPower, HIGH); //Power up faucet
    
    wdt_reset(); //Pet the dog
    delay(100); //Wait for sensor to stabalize before we check the analog value
  }

}

//Plays a random track
//Then turns off audio to reduce hissing
void playRandomTrack()
{
  //Used for sound effects
  char track_name[13];
  byte trackNumber = previousTrack1;

  while(trackNumber == previousTrack1 || trackNumber == previousTrack2) //Don't play the same track as the last donation
  {
    //wdt_reset(); //Pet the dog
    trackNumber = random(1, 10); //(inclusive min, exclusive max)
  }
  //sprintf(track_name, "TRACK%03d.mp3", trackNumber); //Splice the track number into file name
  sprintf(track_name, "%d.mp3", trackNumber); //Splice the track number into file name

  Serial.print("Playing: ");
  Serial.println(track_name);

  //if(MP3player.isPlaying()) MP3player.stopTrack(); //Stop any previous track

  wdt_reset(); //Pet the dog
  //Not sure how long these functions take
//  MP3player.begin();
  MP3player.playMP3(track_name);
  wdt_reset(); //Pet the dog

  //Update the previous variables
  previousTrack2 = previousTrack1;
  previousTrack1 = trackNumber;
  
  playerStopped = false; //Boolean for main loop to turn off MP3 IC
}

//Plays a given track name
//Then turns off audio to reduce hissing
void playTrack(char * track_name)
{
  wdt_reset(); //Pet the dog
  //Not sure how long these functions take
  MP3player.playMP3(track_name);
  wdt_reset(); //Pet the dog

  playerStopped = false; //Boolean for main loop to turn off MP3 IC
}

//Initializes the SD card and checks for an error
void initSD()
{
  //Initialize the SdCard.
  if (!sd.begin(SD_SEL, SPI_HALF_SPEED))
    sd.initErrorHalt();
}

//Sets up all of the initialization for the
//MP3 Player Shield. It runs the begin() function, checks
//for errors, applies a patch if found, and sets the volume/
//stero mode.
void initMP3Player()
{
  MP3player.begin(); // init the mp3 player shield

  //Volume level 5/6/2015 - Original setting
  MP3player.setVolume(30, 30); // MP3 Player volume 0=max, 255=lowest (off)
  //MP3player.setVolume(0, 0); //Setting this to zero causes a lot of audio glitches around start/end of tracks

  MP3player.setMonoMode(1); // Mono setting: 0=off, 1 = on, 3=max
}
