varying vec3 norm;
varying vec3 ePos;

void main()
{	
    float fr = dot(ePos, norm);
	
	//gl_FragColor = vec4(1., fr, fr, 1.0);// vec4(1.0, 0.0,0.0,1.0);
	gl_FragColor = vec4(vec3(fr), 1.0);// vec4(1.0, 0.0,0.0,1.0);
} 