/* BASED ON XBEE-ARDUINO RX EXAMPLE,
which is copyright (c) 2009 Andrew Rapp. All rights reserved.

****** This example is for Series 2 XBee ****** 

Written by Brett Renfer (robotconscience.com)
for the Rockwell LAB (http://lab.rockwellgroup.com)
*/

/*********************************************************************
  LIBRARIES
*********************************************************************/

//INCLUDE XBEE LIBRARY
// http://code.google.com/p/xbee-arduino/
#include <XBee.h>

//INCLUDE NewSoftSerial library
// http://arduiniana.org/libraries/newsoftserial/
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
  // Connect Arduino pin 9 to TX of usb-serial device or RX pin (0) of another Arduino
  uint8_t ssRX = 9;
  // Connect Arduino pin 10 to RX of usb-serial device or TX pin (1) of another Arduino
  uint8_t ssTX = 10;
  // Remember to connect all devices to a common Ground: XBee, Arduino and USB-Serial device
  NewSoftSerial nss(ssRX, ssTX);

/*********************************************************************
  DEBUGGING VARS
*********************************************************************/

  //verbose = debug mode
  boolean verbose = false;
  ZBTxStatusResponse txStatus = ZBTxStatusResponse();

/*********************************************************************
  SETUP 
*********************************************************************/
  void setup() {  
    // start serial + xbee
    // this speed is set in X-CTU (default is 9600)
    xbee.begin(9600);
    nss.begin(9600);
    
    if (verbose) nss.println("starting up!");
  }
  
  
/*********************************************************************
  LOOP
*********************************************************************/
  
  // continuously reads packets, looking for ZB Receive
  
  void loop() {         
  
    // send the serial 'v' if you want to switch to debug mode
    // this is pretty useful if you want to use the arduino serial monitor
    while (nss.available()){
      char inByte = nss.read();
      if (inByte == 'v') verbose = !verbose;
    }
    
    // check XBee        
    xbee.readPacket();
    
    //got xbee packet
    if (xbee.getResponse().isAvailable()) {
      if (verbose) nss.println("got something");
           
      // got a zb rx packet
      if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
        
        // now fill our zb rx object
        xbee.getResponse().getZBRxResponse(rx);    
        
        /**************** NORMAL MODE ******************/
        
        // print packets separated by :
        // in format remoteAddressLow:value ... :value     
        if (!verbose){
          //you will most likely not need the serial high value, 
          // but if you do, uncomment this
          // nss.print(rx.getRemoteAddress64().getMsb(), HEX);
          nss.print(rx.getRemoteAddress64().getLsb(), HEX);
          nss.print(":");
          
          //figure how much data in the payload + print each byte as hex data
          int len = rx.getDataLength();
          for (int i = 0; i < len; i++) {
            nss.print(rx.getData()[i], HEX);
            if (i+1 < len) nss.print(":");
          }
          nss.println("");
        }
        
        /**************** DEBUG MODE ******************/
        
        else if (verbose){
          nss.println("Got an rx packet!");
          if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) {
              // the sender got an ACK
              nss.println("packet acknowledged");
          } else {
            nss.println("packet not acknowledged");
          }
        
          //verbose: print out all available data from rec'd packets
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
        }
      }    
      
    //error: if there's any junk data in the packet, it will return false
    // for the isAvailable() call and return true for isError()
    } else if (xbee.getResponse().isError()) {
      if (verbose){
        nss.print("oh no!!! error code:");
        nss.println(xbee.getResponse().getErrorCode());
      }
    }     
    delay(10);
  }
