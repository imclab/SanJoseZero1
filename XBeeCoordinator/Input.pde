class Message
{
  int threshold;
  String messageString; 
};
class Input
{
  int[] values;
  String name;
  String address;
  PFont font;
  int x, y, width, height;
  int fontSize;
  int deadFrames = 0;
  boolean newFrame;
  boolean[] newThings;
  boolean sendSound;
  boolean doesSendOsc;
  OscP5 oscP5;
  NetAddress demoLocation;
  
  ArrayList[] messages;
  
  //buttons
  
  Button oscButton;
  Button soundButton;
  
  //fills
  
  color dFill = color(50,50,50);
  color aFill = color(150,150,0);
  
/**********************************************************
  SETUP
**********************************************************/
  
  Input(OscP5 _oscP5, NetAddress _demoLocation, String _address, int numValues, PFont _font, int _fontSize ){
    oscP5 = _oscP5;
    demoLocation = _demoLocation;
    values = new int[numValues];
    newThings = new boolean[numValues];
    messages = new ArrayList[numValues];
    x = y = 0;
    address = _address;
    font = _font;
    fontSize = _fontSize;
    newFrame = false;
    sendSound = false;
    
    oscButton = new Button( font, 10 );
    oscButton.width = 75;
    oscButton.height = 20;
    oscButton.name = "send OSC";
    
    soundButton = new Button( font, 10 );
    soundButton.width = 75;
    soundButton.height = 20;
    soundButton.name = "play sound";
    for (int i=0; i<numValues; i++){
      messages[i] = new ArrayList();
    }
  };
  
  void addMessage( String ms, int thresh, int index){
    Message m = new Message();
    m.messageString = ms;
    m.threshold = thresh;
    messages[index].add(m);    
  };
  
/**********************************************************
  DRAW
**********************************************************/
  
  void draw(){
    if (doesSendOsc){
      oscButton.x = x+8;
      oscButton.y = y+height-28;
      soundButton.x = x+oscButton.width+16;
      soundButton.y = y+height-28;
    } else {
      soundButton.x = x+8;
      soundButton.y = y+height-28;
    };
    
    noStroke();
    pushMatrix();
      translate(x, y);
      if (newFrame || sendSound) fill(aFill);
      else fill(dFill);
          
      rect(0,0,width,height);
      
      fill(200);
      textFont( font, fontSize );
      text(name.toUpperCase()+"\n", 8, fontSize+4);
      fill(100);
      text("Address: "+address+"\n", 8, (fontSize+4)*2);
      
      for (int i=0; i<values.length; i++){
        text("Value: "+values[i], 8, (fontSize+4)*(i+3));
      }
      
      translate(75,150);
      fill(90);
      //ellipse(0,0, 100,100);
      //rotateZ(radians((float) value/1023.0*360.0));
      fill(75);
      //rect(-10, -70, 20, 50);
      
    popMatrix();
    if (doesSendOsc) oscButton.draw();
    soundButton.draw();
    
    //deadFrames++;
    //reset vars
  };
  
/**********************************************************
  UTILS
**********************************************************/

  void setName (String _name){
    name = _name;
  }

  boolean update( int[] vals ){
    newFrame = false;
    for (int i=0; i<vals.length; i++){
      if ((values[i] != vals[i] && vals[i] > 0) || values[i] == 1){
        newThings[i] = true;
        newFrame = true;
      } else {
        newThings[i] = false;
      }
    }
    values = vals;
    deadFrames = 0;
    return newFrame;
  }
  
  void send(){
    for (int i=0; i<newThings.length; i++){
      if (newThings[i] == true){
        OscMessage msg = new OscMessage(getMessageString(i));
        msg.add(values[i]);
        oscP5.send(msg, demoLocation);
          
        newThings[i] = false;
      }
    }  
  }
  
  String getMessageString( int index ){
      int which = 0;
      int lastThresh = -9999;
      for (int i=0; i<messages[index].size(); i++){
        Message m = (Message) messages[index].get(i);
        if (values[index] >= m.threshold && m.threshold > lastThresh){
          which = i;
          lastThresh = m.threshold;
        }
      };
      
      return ((Message) messages[index].get(which)).messageString;
  };
  
  boolean mousePressed( int x, int y){
    if (doesSendOsc && oscButton.mousePressed(x,y)){
      newFrame = true;
      return true;
    } else if (soundButton.mousePressed(x,y)){
      sendSound = true;
      newFrame = false;
      return true;
    }
    return false;
  }  
}
