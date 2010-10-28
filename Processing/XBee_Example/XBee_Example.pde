import processing.opengl.*;
import processing.serial.*;

/**********************************************************
  SERIAL VARS
**********************************************************/

  Serial port;
  
  String buff = "";
  int NEWLINE = 10;

/**********************************************************
  LAYOUT VARS
**********************************************************/
  
  int spacingX = 200;
  int spacingY = 300;
  
  PFont font;
  int fontSize = 20;
  
  //arraylist of Input objects
  ArrayList inputs;

/**********************************************************
  SETUP
**********************************************************/
  
  void setup () {
    size (1024, 768, OPENGL);
    font = loadFont("Rockwell-Light-20.vlw");
    inputs = new ArrayList();
    
    println("Available serial ports:");
    println(Serial.list());   
    port = new Serial(this, Serial.list()[0], 9600); 
  }
  
/**********************************************************
  DrAW
**********************************************************/

  void draw(){ 
    //layout stuff
    smooth();
    background(150);
    int curX = 0;
    int curY = 0;
    
    //read incoming serial
    while (port.available() > 0) serialEvent(port.read());
    
    //draw all active inputs
    for (int i=inputs.size()-1; i>=0; i--){
      Input input = (Input) inputs.get(i);
      input.x = curX;
      input.y = curY;
      input.draw();
      
      //if we haven't heard from this input for a while,
      // delete it
      if (input.deadFrames > 10000){ 
        inputs.remove(i);        
      } else {
       //increment layout vars
        curX += 10+spacingX;
        if (curX + spacingX > width){
          curX = 0;
          curY += spacingY;
        }
      }
    }
  
    if (inputs.size() == 0){
      textFont( font, fontSize );      
      fill(200);
      text("no inputs found!",width/2-50,height/2 - fontSize/2.0f);
    }  
  }
  
/**********************************************************
  SERIAL
**********************************************************/
  
  void serialEvent(int serial){
    //in Arduino we are printing out a NEWLINE char at the end of our
    // serial, so read until we hit that character
    if (serial != NEWLINE) {
      // Store all the characters on the line.
      buff += char(serial);
    } else {
      // The end of each line is marked by two characters, a carriage
      // return and a newline.  We're here because we've gotten a newline,
      // but we still need to strip off the carriage return.
      
      if (buff.length() > 0) buff = buff.substring(0, buff.length()-1);
      //if we get a bad read, return so we don't crash our app
      else return;
      
      //split our string based on the ':' character we wrote in Arduino
      String[] values = split(buff, ":");
      
      //error check: are there values to read?
      int [] vals = new int[1];
      
      if (values.length > 1){        
        //this combines the two hex bytes back into one value
        String hexVal = values[1] +""+values[2];
        vals[0] = unhex(hexVal);
      }
      
      //see if it's a new input or not
      Boolean bNewInput = true;      
      for (int i=0; i<inputs.size(); i++){
        Input input = (Input) inputs.get(i);
        
        //first value of array is the address of the XBee
        if (input.address.equals(values[0])){          
          input.update( vals ); //update with new values read
          bNewInput = false;
          break;
        }
      }
      
      //it's a brand new input, so add it!
      if (bNewInput){
        Input newInput = new Input (300 * inputs.size() + 10, 10, (values[0]), font, fontSize);
        newInput.update( vals );
        inputs.add(newInput);
      }
      
      // Clear the value of "buff"
      buff = "";
    }
  }
