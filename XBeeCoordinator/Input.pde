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
  boolean sendSound;
  boolean doesSendOsc;
  
  //buttons
  
  Button oscButton;
  Button soundButton;
  
  //fills
  
  color dFill = color(50,50,50);
  color aFill = color(150,150,0);
  
/**********************************************************
  SETUP
**********************************************************/
  
  Input(String _address, int numValues, PFont _font, int _fontSize ){
    values = new int[numValues];
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
      if (values[i] != vals[i] && vals[i] > 0){
        newFrame = true;
      }
    }
    values = vals;
    deadFrames = 0;
    return newFrame;
  }
  
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
