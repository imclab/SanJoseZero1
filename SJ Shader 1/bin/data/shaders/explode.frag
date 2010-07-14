varying float r;
varying vec2 textureCoords; 
uniform sampler2DRect normalMap; 
uniform sampler2DRect displacementMap; 
void main() { 
	vec4 color = gl_Color;
	color.r *= 5.;
	gl_FragColor = color * texture2DRect(displacementMap, textureCoords); 
}