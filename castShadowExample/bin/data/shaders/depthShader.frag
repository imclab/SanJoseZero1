varying vec3 normal, lightDir;
varying float dist;

uniform sampler2D shadeTex;
uniform sampler2D colorTex;
void main(){
	float intensity;
	vec3 n;
	vec4 color;

	n = normalize(normal);
	//intensity = pow(max(dot(lightDir,n),0.0), 0.8);
	intensity = dot(lightDir,n)*0.5 + 0.5;
	
	color = texture2D(shadeTex, vec2(1.0-intensity));
	color *= texture2D(colorTex, vec2(1.0-(dist*0.45)));
	/*
	if(intensity > 0.99) color = vec4(1.0, 1.0, 1.0,1.0);
	else if(intensity > 0.98) color = vec4(1.0, 1.0, 0.7,1.0);
	
	
	else{
	color = vec4(1.0, 1.0, 0.9, 1.0) * intensity +
			vec4(0.9, 0.1, 0.12, 1.0) * (1.0-intensity);
	}*/

	gl_FragColor = color; // vec4(1.0,0.0,0.0,1.0); //
}

/*varying vec4 pos;

void main()
{	
	float depthVal = pow(pos.z/pos.w, 1.0);
	gl_FragColor =vec4(vec3(depthVal), 1.0);
}*/