uniform sampler2D intex;
uniform sampler2DShadow ShadowMap;
uniform vec2 sampStep;

varying vec4 ShadowCoord;
varying vec3 normal;
varying vec3 lightDir;


float lookup(vec2 offset, vec4 sCoord){
	float sVal = shadow2DProj(ShadowMap , sCoord + vec4(offset, 0.0, 0.0)).x;
	sVal = sVal+0.0001 < sCoord.z ? 0.25 : 1.0;
	return sVal;
}

float remap(float value, float oldMin, float oldMax, float newMin, float newMax){		
	float outVal = newMin + (((value-oldMin)/(oldMax-oldMin)) * (newMax-newMin));	
	return  outVal;
}

void main()
{	
	vec4 color = texture2D(intex, gl_TexCoord[0].st);  vec4(1.0,1.0,1.0, 1.0);
	float shadowColor = 0.150;		
	
	//shadow			
	vec4 sCoord = ShadowCoord / ShadowCoord.w ;	
	float shadowVal = 0.0;			
	int i,j;	
	
	//shadowVal += lookup(sampStep * o * 0.5, sCoord);
	for(i=-1; i<2;i++){
		for(j=-1; j<2; j++){
			vec2 offsetVal = sampStep * vec2(i,j);
			shadowVal += lookup(offsetVal, sCoord);
		}
	}
	shadowVal /= 9.0;
	
	//facingRatio from point to light
	float facRat =  pow( max(0.0, dot(normal, lightDir)), 2.0);	
	//color.xyz += pow(facRat, 16.0)+.3; 
	//color.xyz *= min(max(0.2, facRat), shadowVal);
	color.xyz *= min(max(0.25,facRat), shadowVal);	
	
				
	//gl_FragColor = vec4(normal*.5+.5, 1.0); // color;
	gl_FragColor = color;
}

