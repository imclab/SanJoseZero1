//takes thresholded readings + sends serial based on threshold

/* Based on XBee RX, whcih is copyright (c) 2009 Andrew Rapp. All rights reserved. */
/* This example is for Series 2 XBee */

#include <XBee.h>

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
  THRESHOLD STORAGE VARS
*********************************************************************/
  static const int STORE_VALUES = 64;
  int values[STORE_VALUES];
  int pin0 = 0;
  int micVal = 0;
  int absoluteValue = 0;
  int minimumValue = 0;
  int frame =0;
  
  //change this to adjust the sensitivity
  int sendThreshold = 30;
  
/*********************************************************************
  SETUP
*********************************************************************/
  
  void setup() {  
    xbee.begin(9600);
    //Serial.begin(9600);
    
    minimumValue = 0;
    for (int i=0; i<STORE_VALUES; i++){
      values[i] = 0;
    }
  }

/*********************************************************************
  LOOP
*********************************************************************/

  void loop()
  { 
    
    micVal = analogRead(0);
    
    // for first 64 frames, get a base reading + figure out 
    // a minimum value from there
    if (frame <= STORE_VALUES){
      minimumValue = 0;
      for (int i = 0; i < STORE_VALUES-1; i++){
        values[i] = values[i + 1];
        minimumValue += values[i];
      }
      
      minimumValue += micVal;
      minimumValue /= (float) STORE_VALUES;
      //increment frame
      frame++;
        
      // Add the received value to the array.
      values[STORE_VALUES-1] = micVal;
    } else {
      absoluteValue = abs(micVal - minimumValue);

            
      //is the value above the threshold?
      if (absoluteValue > sendThreshold){
        // break down 10-bit reading into two bytes and place in payload    
        payload[0] = absoluteValue & 0xff;
        xbee.send(zbTx);
      }
    }
   
    delay(5);
  }
