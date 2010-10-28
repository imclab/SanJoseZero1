class Input
{
  int[] values;
  String address;
  PFont font;
  int x, y;
  int fontSize;
  int deadFrames = 0;
  
  Input( int _x, int _y, String _address, PFont _font, int _fontSize ){
    values = new int[1];
    x = _x;
    y = _y;
    address = _address;
    font = _font;
    fontSize = _fontSize;
    textFont( font, fontSize );
  };
  
  //update via an array in case later you want to expand 
  // to more than one sensor reading
  void update( int[] vals ){
    values = vals;
    deadFrames = 0;
  }
  
  //draw
  
  void draw(){
    noStroke();
    pushMatrix();
      translate(x, y);
      
      fill(200);
      text("Address: "+address+"\n", 0, fontSize);
      
      for (int i=0; i<values.length; i++){
        text("Value: "+values[i], 0, fontSize*(i+2));
      }    
    popMatrix();
    deadFrames++;
  };
  
}
