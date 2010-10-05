/* Based on XBee Library Series2_Tx example, 
which is copyright (c) 2009 Andrew Rapp. All rights reserved.

****** This example is for Series 2 XBee ****** 

Written by Brett Renfer (robotconscience.com)
for the Rockwell LAB (http://lab.rockwellgroup.com)
*/

//INCLUDE XBEE LIBRARY
// http://code.google.com/p/xbee-arduino/

#include <XBee.h>

/*********************************************************************
  XBEE VARS
*********************************************************************/

  // create the XBee object
  XBee xbee = XBee();
  
  //build two byte payload: this is where you will store data that 
  // you want to send. increase the size of this to send more data
  uint8_t payload[] = { 0, 0 };
  
  // SH + SL Address of receiving XBee
  // - read this off the back of the XBee you are sending to
  XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x406003b6);
  
  //setup ZBTxRequest with address and payload array
  ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));  
  ZBTxStatusResponse txStatus = ZBTxStatusResponse();
  
/*********************************************************************
  ANALOG READ SETUP
*********************************************************************/

  int readVal = 0;
  int whichPin = 0;
  
  //set this to true to wait for status response from receiving XBee
  boolean bCheckResponse = false; 
  
/*********************************************************************
  SETUP
*********************************************************************/
  
  void setup() {  
    // this speed is set in X-CTU (default is 9600)
    xbee.begin(9600);
  }

/*********************************************************************
  LOOP
*********************************************************************/

  void loop()
  {     
    readVal = analogRead(whichPin);
    
    // break down 10-bit reading into two bytes and place in payload    
    payload[0] = readVal >> 8 & 0xff;
    payload[1] = readVal & 0xff;
    
    //send! 
    xbee.send(zbTx);
    
    
    // if checking response, wait for status response
    if (bCheckResponse){
      // after sending a tx request, we expect a status response
      // wait up to half second for the status response
      if (xbee.readPacket(500)) {
          // got a response!
  
          // should be a znet tx status            	
      	if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
      	   xbee.getResponse().getZBTxStatusResponse(txStatus);
      		
      	   // get the delivery status, the fifth byte
             if (txStatus.getDeliveryStatus() == SUCCESS) {
               // all good! put something here to denote success
             } else {
               //uh-oh, failure... 
               // put something here to denote that packet wasn't received
             }
          }      
      }
     
    // otherwise, short delay before reading again 
    } else {
      delay(15);
    }
  }
