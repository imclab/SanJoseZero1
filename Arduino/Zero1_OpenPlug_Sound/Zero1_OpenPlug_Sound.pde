//takes thresholded readings + sends serial based on threshold

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
  
  uint8_t payload[] = { 0 };
  
  // SH + SL Address of receiving XBee
  XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x406003b6);
  ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
  
  ZBTxStatusResponse txStatus = ZBTxStatusResponse();
  ZBRxResponse rx = ZBRxResponse();
  
 /*********************************************************************
  WAVE SHIELD VARS
*********************************************************************/

  SdReader card;    // This object holds the information for the card
  FatVolume vol;    // This holds the information for the partition on the card
  FatReader root;   // This holds the information for the volumes root directory
  FatReader file;   // This object represent the WAV file 
  WaveHC wave;      // This is the only wave (audio) object, since we will only play one at a time
  
  // time to play each tone in milliseconds
  #define PLAY_TIME 1000
  int t = 0;
  
  #define error(msg) error_P(PSTR(msg)) //Define macro to put error messages in flash memory
  
/*********************************************************************
  THRESHOLD STORAGE VARS
*********************************************************************/

  int pin0 = 0;
  
/*********************************************************************
  SETUP
*********************************************************************/
  
  void setup() {  
    xbee.begin(9600);
    //Serial.begin(9600);
 
    
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

/*********************************************************************
  LOOP
*********************************************************************/

  void loop()
  {
    pin0 = digitalRead(14);
    if (pin0 == 0){
      // break down 10-bit reading into two bytes and place in payload    
      payload[0] = pin0 & 0xff;
      
             // Play the sound
           if(millis()-t > PLAY_TIME && !wave.isplaying){
            t = millis();
            wave.stop();
            openByIndex(0); // open first file
            wave.play();
          }
          
          
      xbee.send(zbTx);      
    }
   
    delay(5);
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
    while(1);
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
    while(1);
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
