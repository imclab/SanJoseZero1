/**
 * Copyright (c) 2009 Andrew Rapp. All rights reserved.
 *
 * This file is part of XBee-Arduino.
 *
 * XBee-Arduino is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * XBee-Arduino is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XBee-Arduino.  If not, see <http://www.gnu.org/licenses/>.
 */

/**********************************************************
  XBEE CODE
**********************************************************/

#include <XBee.h>

/*
This example is for Series 2 XBee
Sends a ZB TX request with the value of analogRead(pin0) and checks the status response for success
*/

// create the XBee object
XBee xbee = XBee();

uint8_t payload[] = { 0, 0, 0, 0, 0, 0 };

// SH + SL Address of receiving XBee
XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x406003b6);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

/*********************************************************************
  WAVE SHIELD VARS
*********************************************************************/

  #include <WaveHC.h>
  #include <WaveUtil.h>

  SdReader card;    // This object holds the information for the card
  FatVolume vol;    // This holds the information for the partition on the card
  FatReader root;   // This holds the information for the volumes root directory
  FatReader file;   // This object represent the WAV file 
  WaveHC wave;      // This is the only wave (audio) object, since we will only play one at a time
  
  // time to play each tone in milliseconds
  #define PLAY_TIME 1000
  int t = 0;
  
  #define error(msg) error_P(PSTR(msg)) //Define macro to put error messages in flash memory

/**********************************************************
  INPUTS
**********************************************************/

  boolean verbose = false;
  long read1[2];
  long read2[2];
  long read3[2];
  long read4[2];
  long read5[2];
  long read6[2];
  
  int pin1 = 14;//2;
  int pin2 = 15;//4;
  int pin3 = 16;//6;
  int pin4 = 17;//8;
  int pin5 = 18;//10;
  int pin6 = 19;//12;

/**********************************************************
  SETUP
**********************************************************/
  
  void setup() {  
    xbee.begin(9600);
    pinMode(pin1, INPUT);
    pinMode(pin2, INPUT);
    pinMode(pin3, INPUT);
    pinMode(pin4, INPUT);
    pinMode(pin5, INPUT);
    pinMode(pin6, INPUT);
    
    //initialize reads to 0
    
    for (int i=0; i<2; i++){
      read1[i] = 0;
      read2[i] = 0;
      read3[i] = 0;
      read4[i] = 0;
      read5[i] = 0;
      read6[i] = 0;
    }
    
    if (verbose) Serial.begin(9600);
    
    //setup wave sheild 
    if (!card.init()) error("card.init");
    
     // enable optimized read - some cards may timeout
    card.partialBlockRead(true);
 
     // Now we will look for a FAT partition!
    uint8_t part;
    for (part = 0; part < 5; part++) {     // we have up to 5 slots to look in
      if (vol.init(card, part)) 
        break;                             // we found one, lets bail
    }
    if (part == 5) {                       // if we ended up not finding one  :(
      putstring_nl("No valid FAT partition!");
      sdErrorCheck();      // Something went wrong, lets print out why
      // then 'halt' - do nothing!
    } else {
      if (!root.openRoot(vol)){
        error("openRoot");
      } else {
        PgmPrintln("Index files");
        indexFiles();
        openByIndex(0); // open first file
        wave.play();
      }
    }
  }

/**********************************************************
  LOOP
**********************************************************/

void loop()
{   
  //shift over reads to last value
  read1[1] = read1[0];
  read2[1] = read2[0];
  read3[1] = read3[0];
  read4[1] = read4[0];
  read5[1] = read5[0];
  read6[1] = read6[0];
  
  // get capactitive reads
  read1[0] = digitalRead(pin1);
  read2[0] = digitalRead(pin2);
  read3[0] = digitalRead(pin3);
  read4[0] = digitalRead(pin4);
  read5[0] = digitalRead(pin5);
  read6[0] = digitalRead(pin6);

   // Play the sound?
   // check for a new value && see if that value is != 0
   if (   (read1[0] != read1[1] && read1 > 0) 
       || (read2[0] != read2[1] && read2 > 0) 
       || (read3[0] != read3[1] && read3 > 0)
       || (read4[0] != read4[1] && read4 > 0)
       || (read5[0] != read5[1] && read5 > 0) 
       || (read6[0] != read6[1] && read6 > 0))
   {
   
     if(millis()-t > PLAY_TIME){
       t = millis();
       wave.stop();
       openByIndex(0); // open first file
       wave.play();
      }
   }

  if (verbose){
    Serial.print(read1[0]);
    Serial.print(":");
    Serial.print(read2[0]);
    Serial.print(":");
    Serial.print(read3[0]);
    Serial.print(":");
    Serial.print(read4[0]);
    Serial.print(":");
    Serial.print(read5[0]);
    Serial.print(":");
    Serial.println(read6[0]);
  } else {
    
    payload[0] = read1[0] & 0xff;
    
    payload[1] = read2[0] & 0xff;
    
    payload[2] = read3[0] & 0xff;
    
    payload[3] = read4[0] & 0xff;
    
    payload[4] = read5[0] & 0xff;
    
    payload[5] = read6[0] & 0xff;    
    xbee.send(zbTx);
  }
  delay(30);
}

/*********************************************************************
  WAVE FUNCTIONS
*********************************************************************/
  
  /////////////////////////////////// HELPERS
  /*
   * print error message and halt
   */
  void error_P(const char *str)
  {
    PgmPrint("Error: ");
    SerialPrint_P(str);
    sdErrorCheck();
    //while(1);
  }
  /*
   * print error message and halt if SD I/O error, great for debugging!
   */
  void sdErrorCheck(void)
  {
    if (!card.errorCode()) return;
    PgmPrint("\r\nSD I/O error: ");
    Serial.print(card.errorCode(), HEX);
    PgmPrint(", ");
    Serial.println(card.errorData(), HEX);
    //while(1);
  }
  
  // Number of files.
  #define FILE_COUNT 16
  
  // Files are 'touch tone phone' DTMF tones, P = #, S = *
  // Most phones don't have A, B, C, and D tones.
  // file names are of the form DTMFx.WAV where x is one of
  // the letters from fileLetter[]
  char fileLetter[] =  {'0', '1', '2', '3', '4', '5', '6', 
        '7', '8', '9', 'A', 'B', 'C', 'D', 'P', 'S'}; 
        
  // index of DTMF files in the root directory
  uint16_t fileIndex[FILE_COUNT];
  /*
   * Find files and save file index.  A file's index is is the
   * index of it's directory entry in its directory file. 
   */
  void indexFiles(void)
  {
    char name[10];
    
    // copy flash string to RAM
    strcpy_P(name, PSTR("DTMFx.WAV"));
    
    for (uint8_t i = 0; i < FILE_COUNT; i++) {
      
      // Make file name
      name[4] = fileLetter[i];
      
      // Open file by name
      if (!file.open(root, name)) error("open by name");
      
      // Save file's index (byte offset of directory entry divided by entry size)
      // Current position is just after entry so subtract one.
      fileIndex[i] = root.readPosition()/32 - 1;   
    }
    PgmPrintln("Done");
  }
  /*
   * Play file by index and print latency in ms
   */
  void  openByIndex(int i)
  {
    // start time
    uint32_t t = millis();
    
    // open by index
    if (!file.open(root, fileIndex[i])) {
      error("open by index");
    }
    
    // create and play Wave
    if (!wave.create(file)) error("wave.create");
    //wave.play();
    
    // print time to open file and start play
    //Serial.println(millis() - t);
    
    // stop after PLAY_TIME ms
    //while((millis() - t) < PLAY_TIME);
    //wave.stop();
    
    // check for play errors
    sdErrorCheck();
    PgmPrintln("Done");
  }
  /*
   * Play file by name and print latency in ms
   */
  void playByName(void)
  {
    char name[10];
    
    // copy flash string to RAM
    strcpy_P(name, PSTR("DTMFx.WAV"));
    
    for (uint8_t i = 0; i < FILE_COUNT; i++) {
      // start time
      uint32_t t = millis();
      
      // make file name
      name[4] = fileLetter[i];
      
      // open file by name
      if (!file.open(root, name)) error("open by name"); 
      
      // create wave and start play
      if (!wave.create(file))error("wave.create");
      wave.play();
      
      // print time
      Serial.println(millis() - t);
      
      // stop after PLAY_TIME ms
      while((millis() - t) < PLAY_TIME);
      wave.stop();
      
      // check for play errors
      sdErrorCheck();
    }
    PgmPrintln("Done");
  }

