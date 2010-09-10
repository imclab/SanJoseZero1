#extension GL_ARB_texture_rectangle : enable
uniform sampler2DRect src_tex_unit0;
uniform float mult;
float mixAmt = .5;


void main() { 
	vec2 st = gl_TexCoord[0].st;
	vec4 color = texture2DRect(src_tex_unit0, st);
   //gl_FragColor = gl_Color * color * mult;// * texture2DRect(texture, textureCoords); 
  // gl_FragColor = gl_Color * texture2DRect(texture, textureCoords); 
	
	vec4 base	= gl_Color;
	//color	= mix(color, base, mixAmt );
	
	gl_FragColor = color * mult;
}