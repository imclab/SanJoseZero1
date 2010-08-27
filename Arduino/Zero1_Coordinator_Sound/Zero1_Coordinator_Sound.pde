/* BASED ON XBEE-ARDUINO RX EXAMPLE, which is Copyright (c) 2009 Andrew Rapp. All rights reserved. */
/* This is for Series 2 XBee */ 

#include <XBee.h>
#include <NewSoftSerial.h>

/*********************************************************************
  SETUP MAIN RECEIVER
*********************************************************************/

  XBee xbee = XBee();
  XBeeResponse response = XBeeResponse();
  // create reusable response objects for responses we expect to handle 
  ZBRxResponse rx = ZBRxResponse();
  ModemStatusResponse msr = ModemStatusResponse();
  

/*********************************************************************
  SETUP NSS PORT
*********************************************************************/
  
  // Define NewSoftSerial TX/RX pins
  // Connect Arduino pin 9 to TX of usb-serial device
  uint8_t ssRX = 9;
  // Connect Arduino pin 10 to RX of usb-serial device
  uint8_t ssTX = 10;
  // Remember to connect all devices to a common Ground: XBee, Arduino and USB-Serial device
  NewSoftSerial nss(ssRX, ssTX);

/*********************************************************************
  SETUP OBJECTS FOR EACH RECEIVING XBEE
*********************************************************************/
  
  static const int NUM_XBEES = 6;
  
  uint8_t payload[NUM_XBEES][1];// = { 0 };

  // SH + SL Address of each receiving XBee
  uint32_t XBeeAddressesHIGH[NUM_XBEES];
  uint32_t XBeeAddressesLOW[NUM_XBEES];
  
  //objects for XBee library
  XBeeAddress64 xbeeAddressObject[NUM_XBEES] ;// = XBeeAddress64(0x0013a200, 0x403e0f30);
  ZBTxRequest zbTx[NUM_XBEES]; // = ZBTxRequest(addr64, payload, sizeof(payload));

/*********************************************************************
  DEBUGGING VARS
*********************************************************************/

  //verbose = debug mode
  boolean verbose = false;
  boolean bCheckSerial = false;
  ZBTxStatusResponse txStatus = ZBTxStatusResponse();

/*********************************************************************
  SETUP 
*********************************************************************/
  void setup() {  
    // start serial + xbee
    xbee.begin(9600);
    nss.begin(9600);
    
    //twitter
    XBeeAddressesHIGH[0] = 0x0013a200;
    XBeeAddressesLOW[0]  = 0x405DAF88;
    
    //flickr
    XBeeAddressesHIGH[1] = 0x0013a200;
    XBeeAddressesLOW[1]  = 0x405DAFA5;
    
    //4square
    XBeeAddressesHIGH[2] = 0x0013a200;
    XBeeAddressesLOW[2]  = 0x405DAF97;
    
    //hopscotch
    XBeeAddressesHIGH[3] = 0x0013a200;
    XBeeAddressesLOW[3]  = 0x405DAF99;
    
    //traffic
    XBeeAddressesHIGH[4] = 0x0013a200;
    XBeeAddressesLOW[4]  = 0x405DAF9A;
    
    //cdm
    XBeeAddressesHIGH[5] = 0x0013a200;
    XBeeAddressesLOW[5]  = 0x405DAFB1;
        
    //setup each XBee
    for (int i=0; i<NUM_XBEES; i++){
      //init payload
      payload[i][0] = 0;
      xbeeAddressObject[i] = XBeeAddress64( XBeeAddressesHIGH[i], XBeeAddressesLOW[i] );
      zbTx[i] = ZBTxRequest(xbeeAddressObject[i], payload[i], sizeof(payload[i]));      
      if (verbose){
        nss.print("setup ");
        nss.print(XBeeAddressesHIGH[i], HEX);
        nss.print(":");
        nss.println(XBeeAddressesLOW[i],HEX);
      }
    };
    
    delay(100);
    if (verbose) nss.println("starting up!");
  }
  
  
/*********************************************************************
  LOOP
*********************************************************************/
  
  // continuously reads packets, looking for ZB Receive or Modem Status
  void loop() {
           
      // check XBee
        
        xbee.readPacket();
        
        //got xbee packet
        if (xbee.getResponse().isAvailable()) {
          // got something 
           if (verbose) nss.println("got something");
               
          if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
            // got a zb rx packet
            
            // now fill our zb rx class
            xbee.getResponse().getZBRxResponse(rx);
          
            if (verbose) nss.println("Got an rx packet!");
                
            if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) {
                // the sender got an ACK
                if (verbose) nss.println("packet acknowledged");
            } else {
              if (verbose) nss.println("packet not acknowledged");
            }
            
            if (verbose) {
              nss.print("checksum is ");
              nss.println(rx.getChecksum(), HEX);
    
              nss.print("packet length is ");
              nss.println(rx.getPacketLength(), DEC);
            
              nss.print("remote address is ");
              nss.println(rx.getRemoteAddress16(), DEC);
            
              for (int i = 0; i < rx.getDataLength(); i++) {
                nss.print("payload [");
                nss.print(i, DEC);
                nss.print("] is ");
                nss.println(rx.getData()[i], HEX);
              }
              
              for (int i = 0; i < xbee.getResponse().getFrameDataLength(); i++) {
                nss.print("frame data [");
                nss.print(i, DEC);
                nss.print("] is ");
                nss.println(xbee.getResponse().getFrameData()[i], HEX);
              }
              
            //not debug: print packets separated by :
            // in format id:value ... :value        
            } else {
              nss.print(rx.getRemoteAddress64().getLsb(), HEX);
              nss.print(":");
              int len = rx.getDataLength();
              for (int i = 0; i < len; i++) {
                nss.print(rx.getData()[i], HEX);
                if (i+1 < len) nss.print(":");
              }
              nss.println("");
            }
          }
        } else if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE){          
          xbee.getResponse().getZBTxStatusResponse(txStatus);
    		
    	   // get the delivery status, the fifth byte
           if (txStatus.getDeliveryStatus() == SUCCESS) {
             if (verbose) nss.println("send success");
           } else {
             if (verbose) nss.println("send beefed");
           }
          
        //error?
        } else if (xbee.getResponse().isError()) {
          nss.print("oh no!!! error code:");
          nss.println(xbee.getResponse().getErrorCode());
        }
          
        //  check serial to see if we are going to send anything
        while (nss.available() > 0){
          char inByte = nss.read();
          
          if (verbose){
            nss.println("sending to LED");
          }
          //error checking: make sure within the range 0 -> NUM_XBEES
          //if (inByte >= 0 && inByte < NUM_XBEES){
            if (inByte == 'v') verbose = !verbose;
            //twitter
            else if (inByte == 't'){              
              payload[0][0] = 0 & 0xff;   
              xbee.send(zbTx[0]);
            if (verbose) nss.println("send twitter");
            //flickr
            } else if (inByte == 'f'){
              payload[1][0] = 0 & 0xff;   
              xbee.send(zbTx[1]);
            if (verbose) nss.println("send flickr");
              
            //foursquare
            } else if (inByte == 's'){
              payload[2][0] = 0 & 0xff;   
              xbee.send(zbTx[2]);
              
            //hopscotch
            } else if (inByte == 'h'){
              payload[3][0] = 0 & 0xff;   
              xbee.send(zbTx[3]);
              
            //tracffic
            } else if (inByte == 'r'){
              payload[4][0] = 0 & 0xff;   
              xbee.send(zbTx[4]);
              
            //cdm
            } else if (inByte == 'c'){
              payload[5][0] = 0 & 0xff;   
              xbee.send(zbTx[5]);
            } 
          //};
        }
  }
