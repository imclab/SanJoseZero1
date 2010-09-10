import processing.serial.*;
import oscP5.*;
import netP5.*;

/**********************************************************
  XML IMPORT
**********************************************************/
  
  XMLElement xml;
  
/**********************************************************
  SERIAL VARS
**********************************************************/

  Serial port;
  String portnum;
  String input;
  
  String buff = "";
  int receivedLines = 0;
  int bufferedLines = 0;
  int NEWLINE = 10;

/**********************************************************
  XBEE DEFAULT VARS
**********************************************************/
  //ids
  String [] ids = new String[10];

/**********************************************************
  OSC VARS
  
**********************************************************/

  OscP5 oscP5;
  NetAddress demoLocation;
  String host = "localhost";
  int listenPort = 12002;
  int sendPort = 12345;
  boolean bSerial;

/**********************************************************
  LAYOUT VARS
**********************************************************/
  
  int spacingX = 234;
  int spacingY = 242;
  
  PFont font;
  int fontSize = 18;
  ArrayList inputs;

/**********************************************************
  SETUP
**********************************************************/
  
  void setup () {
    size (1024, 768);
    font = loadFont("Rockwell-Light-20.vlw");
    inputs = new ArrayList();
    
    //load xml
    xml = new XMLElement(this, "settings.xml");
    XMLElement oscSettings = xml.getChild("osc");
    
    println(oscSettings.getChildCount());
    
    println("Available serial ports:");
    println(Serial.list());   
    if (Serial.list().length > 4){
      port = new Serial(this, Serial.list()[0], 9600);
     bSerial = true; 
    } else {
      bSerial = false;
    }
   
    String nHost = oscSettings.getChild("host").getContent();
    if (nHost == null){
      nHost = host;
    }
        
    int nListenPort = Integer.parseInt(oscSettings.getChild("send").getContent());
    if (nListenPort == 0){
      nListenPort = listenPort;
    }
    
    int nSendPort = Integer.parseInt(oscSettings.getChild("listen").getContent());
    if (nSendPort == 0){
      nSendPort = sendPort;
    }
    
    oscP5 = new OscP5(this, nListenPort);
    demoLocation = new NetAddress(nHost, nSendPort);
    
    //setup inputs    
      //default values...
    ids[0] = "405DAF8D"; //megaphone
    ids[1] = "405DAF9B"; //birdfeeder
    ids[2] = "405DAF9E"; //picnic
    ids[3] = "405DAFAA"; //openplug
    ids[4] = "405DAF88"; //twitter
    ids[5] = "405DAFA5"; //flickr
    ids[6] = "405DAF97"; //foursquare
    ids[7] = "405DAF99";
    ids[8] = "405DAF9A";
    ids[9] = "405DAFB1";
    
    XMLElement sensors = xml.getChild("sensors");

    for (int i=0; i<sensors.getChildCount(); i++){
      XMLElement sensor = sensors.getChild(i);
      String name = sensor.getChild("name").getContent();
      String id   = sensor.getChild("id").getContent();
      int numValues   = Integer.parseInt(sensor.getChild("values").getContent());
      int sendOsc = Integer.parseInt(sensor.getChild("sendosc").getContent());
      
      println(name+":"+id+":"+sendOsc);
      
      Input input = new Input (oscP5, demoLocation, id, numValues, font, fontSize);
      input.setName(name);
      input.width = spacingX;
      input.height = spacingY;
      
      //get messages to send via OSC
      
      XMLElement[] valueMessages = sensor.getChildren("value");
      for (int j=0; j<valueMessages.length; j++){
        for (int k=0; k<valueMessages[j].getChildCount(); k++){
          XMLElement message = valueMessages[j].getChild(k);
          input.addMessage( message.getChild("string").getContent(), 
          Integer.parseInt(message.getChild("threshold").getContent()), j);
          
          println(message.getChild("string").getContent()+":"+Integer.parseInt(message.getChild("threshold").getContent())+":"+j);
        }
      }
            
      if (sendOsc == 0) input.doesSendOsc = false;
      else input.doesSendOsc = true;
      if (input.doesSendOsc==true) input.dFill += 20;
      inputs.add(input);      
    }
  }
  
/**********************************************************
  DrAW
**********************************************************/

  void draw(){ 
    smooth();
    background(150);
    int curX = 50;
    int curY = 10;
    
    //read serial
    if (bSerial) while (port.available() > 0) serialEvent(port.read());

    //draw text labels
    pushMatrix();
      translate(10, spacingY+10);
      rotate(radians(-90));
      fill(100,100,100);
      rect(0,0,spacingY, 30);
      fill(200);
      textFont( font, fontSize );
      text("SENSOR INPUT", 8, fontSize + 4);
    popMatrix();
    
    pushMatrix();
      translate(10, spacingY*2+20);
      rotate(radians(-90));
      fill(100,100,100);
      rect(0,0,spacingY, 30);
      fill(200);
      textFont( font, fontSize );
      text("DIGITAL INPUT", 8, fontSize + 4);
    popMatrix();

    // draw debug + check whether to send serial 
    // sound commands
    pushMatrix();
      //translate(40,0);
      for (int i=0; i<inputs.size(); i++){
        Input input = (Input) inputs.get(i);
        input.x = curX;
        input.y = curY;
        input.draw();
       
        //do we play a sound this frame?
        // note: special cases for foursquare (sends 's')
        // and traffic (sends 'r')
        if (input.sendSound==true){
          if (input.name.equals("foursquare")){
            if (bSerial) port.write('s');
            println("play sound: s");
          } else if (input.name.equals("traffic")){
            if (bSerial) port.write('r');
            println("play sound: s");
          } else {
            if (bSerial) port.write(input.name.charAt(0));
            println("play sound: "+input.name.charAt(0));
          }
        
        //are we sending OSC this frame?
        } else if (input.newFrame==true){
          input.send();
          println("sending "+"/pluginplay/"+input.name);
        };
        
        input.sendSound = false;
        input.newFrame = false;
       
        //increment layout variables      
        curX += 10+spacingX;
        if (curX + spacingX > width || i == 3){
          curX = 50;
          curY += spacingY + 10;
        }
      }  
      
      pushMatrix();
        translate(curX, curY);
        fill(200);
        textFont( font, fontSize );
        text("sending OSC to "+host+", "+sendPort+"\n", 8, fontSize+4);
        text("listen for OSC at "+listenPort+"\n", 8, fontSize*2+8);
      popMatrix();
      
    popMatrix();
  }
  
/**********************************************************
  GET OSC  
**********************************************************/
  
void oscEvent(OscMessage theOscMessage) {
  /* check if theOscMessage has the address pattern we are looking for. */
  
  if(theOscMessage.checkAddrPattern("/pluginplay/sound")==true) {
      /* parse theOscMessage and extract the values from the osc message arguments. */
      String name = theOscMessage.get(0).stringValue();
      println("caught "+name);
      //find name in array      
      for (int i=inputs.size()-1; i>=0; i--){
        Input input = (Input) inputs.get(i);
        if (input.name.equals(name)){
          input.sendSound = true;
          break;
        }
      }
      return;
  } 
  println("### received an osc message. with address pattern "+theOscMessage.addrPattern());
}
  
/**********************************************************
  SERIAL
**********************************************************/
  
  void serialEvent(int serial){
    if (serial != NEWLINE) {
      // Store all the characters on the line.
      buff += char(serial);
    } else {
      // The end of each line is marked by two characters, a carriage
      // return and a newline.  We're here because we've gotten a newline,
      // but we still need to strip off the carriage return.
            
      if (buff.length() > 0) buff = buff.substring(0, buff.length()-1);
      else return;
      
      String[] values = split(buff, ":");
      Boolean found = false;
      
      //println("got "+values[0]);
            
      for (int i=0; i<inputs.size(); i++){
        Input input = (Input) inputs.get(i);
        if (input.address.equals(values[0])){
          
          boolean newFrame = false;
          int[] vals = new int[1];
          
          if (input.name.equals("picnictable")){
            if (values.length > 3){
               vals = new int[6];     
               for (int j=1; j<values.length; j++){
                  vals[j-1] = unhex(values[j]);
               }
               newFrame = input.update( vals );//unhex(hexVal) );
            //only 3 values: so it's mic or birdfeeder
            } 
          } else if (input.name.equals("birdfeeder")){
            if (values.length > 2){
              vals = new int[1];
              String hexVal = values[1] +""+values[2];
              vals[0] = unhex(hexVal);
              newFrame = input.update( vals );
            }
          } else if (input.name.equals("megaphone")){
            println(values.length);
            if (values.length > 2){
              vals = new int[1];
              String hexVal = values[1]+""+values[2];
              println(values[1]+":"+values[2]+"::"+unhex(hexVal));
              vals[0] = unhex(hexVal);
              newFrame = input.update( vals );
            } else if (values.length > 1){
              vals = new int[1];
              String hexVal = values[1];
              vals[0] = unhex(hexVal);
              newFrame = input.update( vals );
            }
          } else {
            println("unknown value "+input.name);
          }        
          
          break;
        }
      }
      
      /*
      //this is not in use in this prototype, as we are only
      //listening for specific IDs
      if (!found){
        Input newInput = new Input (300 * inputs.size() + 10, 10, (values[0]), font, fontSize);
        if (values.length > 3){
             int[] vals = new int[6];     
             for (int j=1; j<values.length; j++){
                vals[j-1] = unhex(values[j]);
             }
             newInput.update( vals );//unhex(hexVal) );
          //only 3 values: so it's mic or birdfeeder
          } else if (values.length > 2){
            int [] vals = new int[1];
            String hexVal = values[1] +""+values[2];
            vals[0] = unhex(hexVal);
            newInput.update( vals );
          } else if (values.length > 1){
            int [] vals = new int[1];
            String hexVal = values[1];
            vals[0] = unhex(hexVal);
            newInput.update( vals );
          }
        inputs.add(newInput);
      }
      */
      
      // Clear the value of "buff"
      buff = "";
    }
  }
  
/**********************************************************
  DEBUGGING
**********************************************************/
  
  void mousePressed(){
      for (int i=0; i<inputs.size(); i++){
        Input input = (Input) inputs.get(i);
        input.mousePressed(mouseX, mouseY);
      }
  }
