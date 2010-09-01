uniform sampler2D inTex;


void main(){
	vec4 c = texture2D(inTex, gl_TexCoord[0].st);
	gl_FragColor = c ;// vec4(1.0, 0.0, 1.0, 1.0); //
}
