class Button
{
  int x, y, width, height;
  String name;
  PFont font;
  int fontSize;
  
  color activeColor = color(150,0,0);
  color rollColor = color(150,75,150);
  color deactiveColor = color(50,75,100);
  
  boolean bActive = false;
  boolean bRolled = false;
  
  Button( PFont _font, int _fontSize ){    
    font = _font;
    fontSize = _fontSize;
  };
  
  void draw(){
    bRolled = mouseMoved(mouseX, mouseY);
    
    if (bActive){
      fill(activeColor);
    } else if (bRolled){
      fill(rollColor);
    } else {
      fill(deactiveColor);
    };
    
    bActive = false;
    bRolled = false;
    
    pushMatrix();
      translate(x,y);
      rect(0,0,width,height);
      fill(255,255,255);
      textFont( font, fontSize );
      text(name.toUpperCase(), 4, 4+fontSize);
    popMatrix();
  };
  
  boolean mousePressed(int _x, int _y){
      if (_x >= x && _x <= x + width && 
      _y >= y && _y <= y + height){
        bActive = true;
        return true;
      }
      return false;
  };
  
  boolean mouseMoved( int _x, int _y ){
    if (_x >= x && _x <= x + width && 
      _y >= y && _y <= y + height){
        return true;
      }
      return false;
  };
};
