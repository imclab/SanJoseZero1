uniform sampler2DRect displacementMap; 
uniform sampler2DRect normalMap; 

varying vec3 norm;

void main(void) 	
{    
   vec4 newVertexPos; 
   vec4 dv; 
   vec3 nm; 
   float df; 
    
   gl_TexCoord[0] = gl_MultiTexCoord0; 
    
   dv = texture2DRect( displacementMap, gl_MultiTexCoord0.xy ); 
   nm = texture2DRect( normalMap, gl_MultiTexCoord0.xy ).xyz; 
    
   df = 0.30*dv.x + 0.59*dv.y + 0.11*dv.z; 
    
   newVertexPos = vec4(gl_Normal * df * 80.0, 0.0) + gl_Vertex; 
    
        norm      = normalize(gl_NormalMatrix * ((gl_Normal+nm)/2.0));
   gl_FrontColor = gl_Color; 
   gl_Position = gl_ModelViewProjectionMatrix * newVertexPos * vec4(1.0,-1.0, 1.0,1.0); 
	
}