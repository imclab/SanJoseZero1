//takes thresholded readings + sends serial based on threshold

/* Based on XBee RX, whcih is copyright (c) 2009 Andrew Rapp. All rights reserved. */
/* This example is for Series 2 XBee */

#include <XBee.h>

/*********************************************************************
  XBEE VARS
*********************************************************************/

  // create the XBee object
  XBee xbee = XBee();
  
  uint8_t payload[] = { 0, 0 };
  
  // SH + SL Address of receiving XBee
  XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x406003b6);
  ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
  
  ZBTxStatusResponse txStatus = ZBTxStatusResponse();
  ZBRxResponse rx = ZBRxResponse();
  
/*********************************************************************
  THRESHOLD STORAGE VARS
*********************************************************************/

  int pin0 = 0;
  int micVal = 0;
  int absoluteValue = 0;
  int minimumValue = 0;
  int frame =0;
  
  //change this to adjust the sensitivity
  int sendThreshold = 0;
  
/*********************************************************************
  SETUP
*********************************************************************/
  
  void setup() {  
    xbee.begin(115200);
    //Serial.begin(9600);
    
    minimumValue = 0;
  }

/*********************************************************************
  LOOP
*********************************************************************/

  void loop()
  { 
    
    micVal = analogRead(0);
    minimumValue = analogRead(1);
    if (micVal > minimumValue){
      absoluteValue = abs(micVal - minimumValue);
    } else {
      absoluteValue = 0;
    }      
    //is the value above the threshold?
    if (absoluteValue > sendThreshold){
      // break down 10-bit reading into two bytes and place in payload    
      payload[0] = absoluteValue >> 8 & 0xff;
      payload[1] = absoluteValue & 0xff;
      xbee.send(zbTx);
    }
   
    delay(5);
  }
