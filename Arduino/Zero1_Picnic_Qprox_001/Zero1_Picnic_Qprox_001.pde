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

/**********************************************************
  CAPACITIVE VARS
**********************************************************/

#define resolution 8
#define mains 60 // 60: north america, japan; 50: most other places
#define refresh 2 * 1000000 / mains

/**********************************************************
  INPUTS
**********************************************************/

  int pin0 = 0;
  boolean verbose = false;
  long read1 = 0;
  long read2 = 0;
  long read3 = 0;
  long read4 = 0;
  long read5 = 0;
  long read6 = 0;
  
  int pin1 = 2;
  int pin2 = 4;
  int pin3 = 6;
  int pin4 = 8;
  int pin5 = 10;
  int pin6 = 12;

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
    
    startTimer();
    if (verbose) Serial.begin(9600);
  }

/**********************************************************
  LOOP
**********************************************************/

void loop()
{   
  // get capactitive reads
  read1 = digitalRead(pin1);
  read2 = digitalRead(pin2);
  read3 = digitalRead(pin3);
  read4 = digitalRead(pin4);
  read5 = digitalRead(pin5);
  read6 = digitalRead(pin6);


  if (verbose){
    Serial.print(read1);
    Serial.print(":");
    Serial.print(read2);
    Serial.print(":");
    Serial.print(read3);
    Serial.print(":");
    Serial.print(read4);
    Serial.print(":");
    Serial.print(read5);
    Serial.print(":");
    Serial.println(read6);
  } else {
    
    payload[0] = read1 & 0xff;
    
    payload[1] = read2 & 0xff;
    
    payload[2] = read3 & 0xff;
    
    payload[3] = read4 & 0xff;
    
    payload[4] = read5 & 0xff;
    
    payload[5] = read6 & 0xff;
    
    xbee.send(zbTx);
  }

  // flash TX indicator
  //flashLed(statusLed, 1, 100);
  /*
  // after sending a tx request, we expect a status response
  // wait up to half second for the status response
  if (xbee.readPacket(500)) {
      // got a response!

      // should be a znet tx status            	
  	if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
  	   xbee.getResponse().getZBTxStatusResponse(txStatus);
  		
  	   // get the delivery status, the fifth byte
         if (txStatus.getDeliveryStatus() == SUCCESS) {
          	// success.  time to celebrate
           	flashLed(statusLed, 5, 50);
         } else {
          	// the remote XBee did not receive our packet. is it powered on?
           	flashLed(errorLed, 3, 500);
         }
      }      
  } else {
    // local XBee did not provide a timely TX Status Response -- should not happen
    flashLed(errorLed, 2, 50);
  }
  */
  //delay(30);
}

/**********************************************************
  CAPACITIVE FUNS
**********************************************************/

long timeBack(int pin, byte mask) {
  unsigned long count = 0, total = 0;
  while(checkTimer() < refresh) {
    // pinMode is about 6 times slower than assigning
    // DDRB directly, but that pause is important
    pinMode(pin, OUTPUT);
    PORTB = 0;
    pinMode(pin, INPUT);
    while((PINB & mask) == 0)
      count ++;
    total ++;
  }
  startTimer();
  return (count << resolution) / total;
}

long timeFront(int pin, byte mask) {
  unsigned long count = 0, total = 0;
  while(checkTimer() < refresh) {
    // pinMode is about 6 times slower than assigning
    // DDRB directly, but that pause is important
    pinMode(pin, OUTPUT);
    PORTD = 0;
    pinMode(pin, INPUT);
    while((PIND & mask) == 0)
      count ++;
    total ++;
  }
  startTimer();
  return (count << resolution) / total;
}

extern volatile unsigned long timer0_overflow_count;

void startTimer() {
  timer0_overflow_count = 0;
  TCNT0 = 0;
}

unsigned long checkTimer() {
  return ((timer0_overflow_count << 8) + TCNT0) << 2;
}


