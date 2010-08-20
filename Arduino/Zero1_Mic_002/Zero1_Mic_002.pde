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
  
  int pin0 = 0;
  boolean LED_on = true;
  int micVal = 0;
/*********************************************************************
  SETUP
*********************************************************************/
  
  void setup() {  
    xbee.begin(9600);
    Serial.begin(9600);
    pinMode(3, OUTPUT);
    digitalWrite(3, HIGH);
  }

/*********************************************************************
  LOOP
*********************************************************************/

  void loop()
  { 
    
    //did we get anything from the controller?
    xbee.readPacket(25);
    
    if (xbee.getResponse().isAvailable()) {
      // got something
      
        if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
          LED_on = !LED_on;
          micVal = 9;
          
          // got a zb rx packet
          
          // now fill our zb rx class
          xbee.getResponse().getZBRxResponse(rx);
          
          // we're just sending '0' for now, so don't worry about
          // what's in the packet.
        }
        
    } else {
        
      micVal = analogRead(0);
      // break down 10-bit reading into two bytes and place in payload    
      payload[0] = micVal & 0xff;
        
      xbee.send(zbTx);
      //Serial.println(micVal);
    }
    
    if (LED_on){
      digitalWrite(3, HIGH);
    } else { 
      digitalWrite(3, LOW);
    }
   
    //delay(10);
  }
