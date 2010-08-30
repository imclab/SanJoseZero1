varying vec3 norm;
varying vec3 ePos;

void main()
{	
    float intensity = (pow(dot(ePos, norm), 1.0) * 0.5) + 0.5;
	vec4 color = vec4(1.0, 0.0, 0.0, 1.0)*intensity + vec4(1.0)*(1.0-intensity);
	
	gl_FragColor = color;// vec4(intensity, 0.0, 0.0, 1.0);// vec4(1.0, 0.0,0.0,1.0);
} 