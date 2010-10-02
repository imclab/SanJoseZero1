
/* Based on XBee RX, whcih is copyright (c) 2009 Andrew Rapp. All rights reserved. */
/* This example is for Series 2 XBee */

#include <XBee.h>
#include <WaveHC.h>
#include <WaveUtil.h>

/*********************************************************************
  XBEE VARS
*********************************************************************/

  // create the XBee object
  XBee xbee = XBee();
  ZBRxResponse rx = ZBRxResponse();
  
  boolean LED_on = true;
  boolean verbose = false;
  boolean bVerbose = false;
  
/*********************************************************************
  WAVE SHIELD VARS
*********************************************************************/

  SdReader card;    // This object holds the information for the card
  FatVolume vol;    // This holds the information for the partition on the card
  FatReader root;   // This holds the information for the volumes root directory
  FatReader file;   // This object represent the WAV file 
  WaveHC wave;      // This is the only wave (audio) object, since we will only play one at a time
  
  // time to play each tone in milliseconds
  #define PLAY_TIME 100
  int t = 0;
  
  #define error(msg) error_P(PSTR(msg)) //Define macro to put error messages in flash memory

/*********************************************************************
  SETUP
*********************************************************************/
  
  void setup() {  
    if (bVerbose) Serial.begin(9600);
    
    //setup wave
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
        if (bVerbose) PgmPrintln("Index files");
        indexFiles();
        openByIndex(0); // open first file
        wave.play();
      }
    }
    xbee.begin(115200);
  }

/*********************************************************************
  LOOP
*********************************************************************/

  void loop()
  { 
    
    //did we get anything from the controller?
    xbee.readPacket();
    
    if (xbee.getResponse().isAvailable()) {
      // got something
      
        if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
          // got a zb rx packet
          
        // now fill our zb rx class
          xbee.getResponse().getZBRxResponse(rx);
          
          if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) {
            // the sender got an ACK
             if (bVerbose) Serial.println("packet acknowledged");
          } else {
             if (bVerbose) Serial.println("packet not acknowledged");
          }
          
          if(millis()-t > PLAY_TIME && !wave.isplaying){
            if (bVerbose) Serial.println("play");
            t = millis();
            //wave.stop();
            openByIndex(0); // open first file
            wave.play();
          }
    
          // stop after PLAY_TIME ms
          //while((millis() - t) < PLAY_TIME);
          //wave.stop();
        }
        
     } else if (xbee.getResponse().isError()) {
       if(bVerbose) Serial.print("oh no!!! error code:");
       if(bVerbose) Serial.println(xbee.getResponse().getErrorCode());
       
       if(millis()-t > PLAY_TIME && !wave.isplaying){
            if (bVerbose) Serial.println("play");
            t = millis();
            //wave.stop();
            openByIndex(0); // open first file
            wave.play();
          }
     }
   
    delay(15);
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
    if (bVerbose) PgmPrint("Error: ");
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
    if (bVerbose) PgmPrint("\r\nSD I/O error: ");
    if (bVerbose) Serial.print(card.errorCode(), HEX);
    if (bVerbose) PgmPrint(", ");
    if (bVerbose) Serial.println(card.errorData(), HEX);
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
    if (bVerbose) PgmPrintln("Done");
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
    //sdErrorCheck();
    //PgmPrintln("Done");
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
      if (bVerbose) Serial.println(millis() - t);
      
      // stop after PLAY_TIME ms
      while((millis() - t) < PLAY_TIME);
      wave.stop();
      
      // check for play errors
      sdErrorCheck();
    }
    if (bVerbose) PgmPrintln("Done");
  }
