//uniform sampler2DShadow intex;
//uniform sampler2DShadow ShadowMap;
uniform sampler2D ShadowMap;

varying vec4 ShadowCoord;
varying vec3 normal;
varying vec3 lightDir;
/*
vec2 lookup(vec2 offsets, vec4 sCoord){
	float depth = shadow2DProj(ShadowMap, sCoord + vec4(offsets.x, offsets.y, 0.0, 0.0)).x;	
	float dVal = pow(depth, 0.0005) + 0.002;
	return vec2(dVal < pow(sCoord.z, 0.0005) ? 0.0 : 1.0, depth) ;
}*/

float remap(float value, float oldMin, float oldMax, float newMin, float newMax){		
	float outVal = newMin + (((value-oldMin)/(oldMax-oldMin)) * (newMax-newMin));	
	return  outVal;
}

void main()
{	
	vec4 color = vec4(1.0,1.0,1.0, 1.0);
	float shadowColor = 0.40;		
	
	//shadow			
	vec4 sCoord = ShadowCoord / ShadowCoord.w ;	
	//sCoord.xy = 1.0-sCoord.xy;
	float shadowVal = 0.0;	
	shadowVal = texture2D(ShadowMap, sCoord.xy ).x;
	//shadowVal = pow(shadowVal, 0.3)+0.00002 < pow(sCoord.z, 0.3) ? 0.2 : 1.0;
	shadowVal = shadowVal+0.0002 < sCoord.z ? 0.2 : 1.0;	
	
	//facingRatio from point to light
	float facRat = max(dot(normalize(normal), normalize(lightDir)),0.0);	
	color.xyz *= shadowVal;		
	
	//gl_FragColor = vec4(vec3(shadowVal), 1.0); 
	gl_FragColor = vec4(vec3(1.2) * shadowVal,1.0);
	//gl_FragColor = vec4(color.xyz * facRat, 1.0);
}

