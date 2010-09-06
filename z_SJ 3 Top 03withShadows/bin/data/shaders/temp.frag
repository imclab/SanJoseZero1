uniform sampler2D inTex;
uniform sampler2D tex2;

void main (void)
{
	vec4 texCol = texture2D(inTex, gl_TexCoord[0].st);
	vec4 texCol2 = texture2D(tex2, gl_TexCoord[0].st);
	
	gl_FragColor = texCol2;
	//gl_FragColor = vec4(1.0,0.0,1.0,1.0); // texCol2; 
}


/*
uniform sampler2D inTex;
varying vec3 norm;
varying vec3 ePos;

void main(){
	
	vec3 ct,cf;	
	float fr, af;
	
    fr =  dot(normalize(-ePos), normalize(norm));	//facing ratio
	vec4 color = texture2D(inTex, gl_TexCoord[0].st);//texture color
	
	cf = fr * (gl_FrontMaterial.diffuse).rgb + gl_FrontMaterial.ambient.rgb;
	af = gl_FrontMaterial.diffuse.a;
	
	color.xyz *= max(0.0, fr) ;
	
	vec4 vDiffuse = gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse;
	
	vec4 vAmbient = gl_LightSource[0].ambient * gl_FrontMaterial.ambient;
	
	//gl_FragColor = vec4(1.0,0.0,1.0,1.0); 
	gl_FragColor = color * vDiffuse ; //
}
*/